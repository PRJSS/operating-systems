//
//  main.cpp
//  var1
//
//  Created by Бернат Данила on 01.11.2021.
//

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <limits>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <string>
#include <iomanip>
#include <locale>
using namespace std;

// Имя временного файла для частичных сумм
const char* const TMP_FILE = "tmpfile";


// Точность выводимых данных
const streamsize PRECISION = 8;

// 2*PI
const double M_PIx2 = 2.0 * M_PI;


// Класс - файл
// Будем использовать низкоуровневый ввод/вывод и обернем это в класс

template<class T>
class file
{
    static const size_t data_len = sizeof (T);
public:
    // Конструктор
    file(const char* const name, int flags);
    // Деструктор
    virtual ~file();
    // Запись значений в файл
    void put(T value);
    // Читает из файла. Возвращет false если конец файла
    bool get(T& result);
private:
    int fd;
    file(const file&);
    file & operator=(const file&);
};

// Для ввода целых чисел
int input(const char* const msg);
// Вычисляет i-й член ряда для аргумента х
double elem(double x, int i);
// Вычисляет значение sin(2*PI*i/N) используя n членов ряда
double msin(double arg, int n);

// Тут, думаю, все понятно
#pragma mark MAIN
int main(int argc, char** argv)
{
    ofstream outf("out.txt");
    
    int N = input("N="), n = input("n=");

    // Выведем заголовок таблицы в файл результатов
    // Для этого рассчитаем ширину столбцов
    size_t w_i = log10(N) + 2;
    size_t w_arg = log10(M_PIx2) + PRECISION + 3;
    size_t w_res = PRECISION + 4;
    wstring line(w_i + w_arg + 2 * w_res + 5, L'-');

    try
    {
        for (int i = 0; i < N; ++i)
        {
            double arg = M_PIx2 * i / N;
            double result = msin(arg, n);
            double etalon = sin(arg);

            // Выводим результат
            cout << "y[" << i << "]=" << result << "(" << etalon << ")" << endl;
            outf << "y[" << i << "]=" << result << "(" << etalon << ")" << endl;
        }
    }
    catch (exception& ex)
    {
        cerr << ex.what() << endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        cerr << "Черт! Неизвестное исключение" << endl;
        return EXIT_FAILURE;
    }
    outf.close();
    return EXIT_SUCCESS;
}

int input(const char* const msg)
{
    int res = 0;
    while (true)
    {
        cout << msg;
        cin >> res;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка ввода. Ожидается int." << endl;
        }
        else if (res <= 0)
        {
            cout << "Ожидается положительное число." << endl;
        }
        else
        {
            return res;
        }
    }
}

template<class T>
file<T>::file(const char* const name, int flags)
: fd(-1)
{
    if ((fd = open(name, flags, S_IRUSR | S_IWUSR)) == -1)
    {
        throw runtime_error(strerror(errno));
    };
}

template<class T>
file<T>::~file()
{
    if (fd != -1 && close(fd) == -1)
    {
        throw runtime_error(strerror(errno));
    }
}

template<class T>
void file<T>::put(T value)
{
    if (fd == -1)
    {
        throw runtime_error("Файл не открыт");
    }

    char* buf = reinterpret_cast<char*> (&value);
    size_t len = data_len;
    ssize_t ret;
    // Вообще то для обычных файлов такие ухищрения не нужны
    // write() гарантированно записывает данные, если только не происходит какой-либо
    // серьезной ошибки
    while (len && (ret = write(fd, buf, len)))
    {
        if (ret == -1)
        {
            if (errno != EINTR)
            {
                throw runtime_error(strerror(errno));
            }
            continue;
        }
        buf += ret;
        len -= ret;
    }
}

template<class T>
bool file<T>::get(T& result)
{
    if (fd == -1)
    {
        throw runtime_error("Файл не открыт");
    }

    T value;
    char* buf = reinterpret_cast<char*> (&value);
    size_t len = data_len;
    ssize_t ret;


    while (len && (ret = read(fd, buf, data_len)))
    {
        if (ret == -1)
        {
            if (errno != EINTR)
            {
                throw runtime_error(strerror(errno));
            }
            continue;
        }
        len -= ret;
        buf += ret;
    }
    if (ret == 0)
    {
        return false;
    }
    result = value;
    return true;
}

double elem(double x, int i)
{
    double res = (i & 1) ? -1.0 : 1.0;
    long p = 2l * i + 1;
    while (p)
    {
        res *= x / p--;
    }
    return res;
}

double msin(double arg, int n)
{
    typedef file<double> dfile;

    // Приведем к диапазону 0..2*PI, чтоб увеличить точность.
    // Хотя оно и так лежит в этом диапазоне согласно условия, но в общем случае это надо
    arg -= floor(arg / M_PIx2) * M_PIx2;
    // Вообще то, для уменьшения погрешности, надо приводить к -PI/2..PI/2
    // В условии этого нет. Так что можете удалить, если не надо.
    // Соответственно, погрешность для больших углов будет больше.
    if (arg > M_PI * 3 / 2)
    {
        arg -= M_PIx2;
    }
    else if (arg > M_PI_2)
    {
        arg = M_PI - arg;
    }

    // Откроем файл
    dfile out(TMP_FILE, O_APPEND | O_CREAT | O_TRUNC | O_WRONLY);
    // Считаем члены ряда
    for (int j = 0; j < n; ++j)
    {
        // Создаем процесс
        pid_t pid = fork();
        // Для дочернего процесса
        if (pid == 0)
        {
            try
            {
                // Считаем член
                double val = elem(arg, j);
                // Выводим чего надо по условию
                cout << getpid() << " --> " << val << endl;
                // И записываем в файл член ряда
                out.put(val);
                // Не забываем завершиться
                exit(EXIT_SUCCESS);
            }
            catch (...)
            {
                exit(EXIT_FAILURE);
            }
        }
        else if (pid == -1)
        {
            throw runtime_error(strerror(errno));
        }
    }
    // Ждем пока есть потомки
    errno = 0;
    while (errno != ECHILD)
    {
        wait(0);
    }
    // Открываем файл для чтения
    dfile in(TMP_FILE, O_RDONLY);
    // Сразу удалим его.
    // В Linux так можно делать. На самом деле дисковое пространство не будет освобождено
    // пока все дескрипторы, ссылающиеся на файл, не будут закрыты
    // Не верите - проверьте сами:)
    if (unlink(TMP_FILE) == -1)
    {
        throw runtime_error(strerror(errno));
    }
    // Сумма членов
    double sum = 0;
    double val;
    // Читаем и суммируем пока не конец
    while (in.get(val))
    {
        sum += val;
    }
    return sum;
}
