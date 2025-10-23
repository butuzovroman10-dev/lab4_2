#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>

// Функция для интегрирования (вариант 6): f(x) = x^2 * sin(x)
double f(double x) {
    return x * x * std::sin(x);
}

// Аналитическое решение интеграла x^2 * sin(x) dx
double analytical_solution(double a, double b) {
    auto F = [](double x) {
        return (2 - x*x) * std::cos(x) + 2*x * std::sin(x);
    };
    return F(b) - F(a);
}

// Метод левых прямоугольников
double left_rectangle(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return sum * h;
}

// Метод правых прямоугольников
double right_rectangle(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;
    for (int i = 1; i <= n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return sum * h;
}

// Метод средних прямоугольников
double mid_rectangle(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double x = a + (i + 0.5) * h;
        sum += f(x);
    }
    return sum * h;
}

// Метод трапеций
double trapezoidal(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = (f(a) + f(b)) / 2.0;
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return sum * h;
}

// Метод Симпсона
double simpson(double a, double b, int n) {
    if (n % 2 != 0) n++;
    double h = (b - a) / n;
    double sum = f(a) + f(b);
    
    for (int i = 1; i < n; i += 2) {
        double x = a + i * h;
        sum += 4 * f(x);
    }
    
    for (int i = 2; i < n; i += 2) {
        double x = a + i * h;
        sum += 2 * f(x);
    }
    
    return sum * h / 3.0;
}

// Функция для измерения времени выполнения
template<typename Func>
std::pair<double, long long> measure_time(Func method, double a, double b, int n) {
    auto start = std::chrono::high_resolution_clock::now();
    double result = method(a, b, n);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    return std::make_pair(result, duration.count());
}

// Функция для создания ASCII графика времени выполнения
void print_time_graph(const std::vector<int>& n_values, 
                     const std::vector<std::vector<long long>>& times,
                     const std::vector<std::string>& method_names) {
    const int graph_width = 60;
    const int graph_height = 20;
    
    // Находим максимальное время для масштабирования
    long long max_time = 0;
    for (const auto& method_times : times) {
        for (long long time : method_times) {
            if (time > max_time) max_time = time;
        }
    }
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "ГРАФИК 1: Время выполнения методов от количества разбиений n\n";
    std::cout << std::string(80, '=') << "\n";
    
    // Символы для разных методов
    std::vector<char> symbols = {'#', '@', '*', '+', '~'};
    std::vector<std::string> colors = {"[1;31m", "[1;32m", "[1;33m", "[1;34m", "[1;35m"}; // ANSI colors
    
    // Создаем координатную сетку
    for (int row = graph_height; row >= 0; row--) {
        // Левая шкала (время)
        if (row % 2 == 0) {
            double time_value = (static_cast<double>(row) / graph_height) * max_time;
            std::cout << std::setw(8) << std::setprecision(2) 
                      << (time_value / 1e6) << "M |";
        } else {
            std::cout << "         |";
        }
        
        // Отображаем точки для каждого метода
        for (int col = 0; col < graph_width; col++) {
            double n_log = std::log10(n_values[0]) + 
                          (static_cast<double>(col) / graph_width) * 
                          (std::log10(n_values.back()) - std::log10(n_values[0]));
            double n_value = std::pow(10, n_log);
            
            bool point_drawn = false;
            for (size_t method_idx = 0; method_idx < method_names.size(); method_idx++) {
                // Находим ближайшее значение n
                for (size_t n_idx = 0; n_idx < n_values.size(); n_idx++) {
                    if (std::abs(std::log10(n_values[n_idx]) - n_log) < 0.1) {
                        double time_ratio = static_cast<double>(times[method_idx][n_idx]) / max_time;
                        int time_row = static_cast<int>(time_ratio * graph_height);
                        
                        if (time_row == row) {
                            std::cout << "\033" << colors[method_idx] << symbols[method_idx] << "\033[0m";
                            point_drawn = true;
                            break;
                        }
                    }
                }
                if (point_drawn) break;
            }
            
            if (!point_drawn) {
                if (row == 0) {
                    std::cout << "-"; // Ось X
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << "\n";
    }
    
    // Ось X (логарифмическая шкала для n)
    std::cout << "         +";
    for (int col = 0; col < graph_width; col++) {
        std::cout << "-";
    }
    std::cout << "> n\n          ";
    
    for (int col = 0; col <= graph_width; col += graph_width / 6) {
        double n_log = std::log10(n_values[0]) + 
                      (static_cast<double>(col) / graph_width) * 
                      (std::log10(n_values.back()) - std::log10(n_values[0]));
        int n_value = static_cast<int>(std::pow(10, n_log));
        std::cout << std::setw(8) << n_value;
    }
    std::cout << "\n";
    
    // Легенда
    std::cout << "\nЛегенда:\n";
    for (size_t i = 0; i < method_names.size(); i++) {
        std::cout << "\033" << colors[i] << symbols[i] << "\033[0m - " << method_names[i] << "\n";
    }
}

// Функция для создания ASCII графика ошибок
void print_error_graph(const std::vector<int>& n_values,
                      const std::vector<std::vector<double>>& errors,
                      const std::vector<std::string>& method_names) {
    const int graph_width = 60;
    const int graph_height = 15;
    
    // Находим максимальную ошибку для масштабирования
    double max_error = 1e-10;
    for (const auto& method_errors : errors) {
        for (double error : method_errors) {
            if (error > max_error) max_error = error;
        }
    }
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "ГРАФИК 2: Ошибка методов от количества разбиений n\n";
    std::cout << std::string(80, '=') << "\n";
    
    std::vector<char> symbols = {'1', '2', '3', '4', '5'};
    std::vector<std::string> colors = {"[1;31m", "[1;32m", "[1;33m", "[1;34m", "[1;35m"};
    
    for (int row = graph_height; row >= 0; row--) {
        // Левая шкала (ошибка)
        if (row % 3 == 0) {
            double error_value = std::pow(10, 
                std::log10(max_error) - (static_cast<double>(row) / graph_height) * std::log10(max_error));
            std::cout << std::scientific << std::setprecision(0) 
                      << std::setw(8) << error_value << " |";
        } else {
            std::cout << "         |";
        }
        
        // Отображаем точки для каждого метода
        for (int col = 0; col < graph_width; col++) {
            double n_log = std::log10(n_values[0]) + 
                          (static_cast<double>(col) / graph_width) * 
                          (std::log10(n_values.back()) - std::log10(n_values[0]));
            double n_value = std::pow(10, n_log);
            
            bool point_drawn = false;
            for (size_t method_idx = 0; method_idx < method_names.size(); method_idx++) {
                for (size_t n_idx = 0; n_idx < n_values.size(); n_idx++) {
                    if (std::abs(std::log10(n_values[n_idx]) - n_log) < 0.1) {
                        double error_log = std::log10(errors[method_idx][n_idx]);
                        double max_error_log = std::log10(max_error);
                        int error_row = static_cast<int>((max_error_log - error_log) / max_error_log * graph_height);
                        
                        if (error_row == row && errors[method_idx][n_idx] > 0) {
                            std::cout << "\033" << colors[method_idx] << symbols[method_idx] << "\033[0m";
                            point_drawn = true;
                            break;
                        }
                    }
                }
                if (point_drawn) break;
            }
            
            if (!point_drawn) {
                if (row == 0) {
                    std::cout << "-";
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << "\n";
    }
    
    // Ось X
    std::cout << "         +";
    for (int col = 0; col < graph_width; col++) {
        std::cout << "-";
    }
    std::cout << "> n\n          ";
    
    for (int col = 0; col <= graph_width; col += graph_width / 6) {
        double n_log = std::log10(n_values[0]) + 
                      (static_cast<double>(col) / graph_width) * 
                      (std::log10(n_values.back()) - std::log10(n_values[0]));
        int n_value = static_cast<int>(std::pow(10, n_log));
        std::cout << std::setw(8) << n_value;
    }
    std::cout << "\n";
    
    // Легенда
    std::cout << "\nЛегенда:\n";
    for (size_t i = 0; i < method_names.size(); i++) {
        std::cout << "\033" << colors[i] << symbols[i] << "\033[0m - " << method_names[i] << "\n";
    }
}

// Функция для создания сравнительной гистограммы
void print_comparison_chart(const std::vector<int>& n_values,
                           const std::vector<std::vector<long long>>& times,
                           const std::vector<std::string>& method_names) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "ГРАФИК 3: Сравнительная гистограмма времени выполнения (n=" 
              << n_values.back() << ")\n";
    std::cout << std::string(80, '=') << "\n";
    
    const int bar_width = 40;
    long long max_time = 0;
    for (const auto& method_times : times) {
        if (method_times.back() > max_time) max_time = method_times.back();
    }
    
    for (size_t i = 0; i < method_names.size(); i++) {
        long long time = times[i].back();
        int bar_length = static_cast<int>((static_cast<double>(time) / max_time) * bar_width);
        
        std::cout << std::setw(12) << method_names[i] << " | " 
                  << std::string(bar_length, '#') 
                  << " " << std::setw(10) << time << " ns\n";
    }
}

int main() {
    // Параметры для варианта 6
    double a = 0.0;
    double b = 3.14159; // π
    
    // Значения n для тестирования
    std::vector<int> n_values = {10, 100, 1000, 10000, 100000, 1000000};
    std::vector<std::string> method_names = {
        "Left Rect", "Right Rect", "Mid Rect", "Trapezoid", "Simpson"
    };
    
    // Аналитическое решение
    double analytical = analytical_solution(a, b);
    std::cout << "АНАЛИТИЧЕСКОЕ РЕШЕНИЕ: ∫x²sin(x)dx от " << a << " до " << b << " = " 
              << std::setprecision(10) << analytical << "\n";
    
    // Векторы для хранения результатов
    std::vector<std::vector<double>> results(method_names.size());
    std::vector<std::vector<long long>> times(method_names.size());
    std::vector<std::vector<double>> errors(method_names.size());
    
    // Заполняем векторы методов
    std::vector<double(*)(double, double, int)> methods = {
        left_rectangle, right_rectangle, mid_rectangle, trapezoidal, simpson
    };
    
    std::cout << "\nВыполняются вычисления...\n";
    
    // Выполняем вычисления
    for (size_t method_idx = 0; method_idx < methods.size(); method_idx++) {
        for (int n : n_values) {
            auto [result, time] = measure_time(methods[method_idx], a, b, n);
            results[method_idx].push_back(result);
            times[method_idx].push_back(time);
            errors[method_idx].push_back(std::abs(result - analytical));
        }
    }
    
    // Выводим графики
    print_time_graph(n_values, times, method_names);
    print_error_graph(n_values, errors, method_names);
    print_comparison_chart(n_values, times, method_names);
    
    // Создаем CSV файл с результатами
    std::ofstream csv_file("integration_results_with_graphs.csv");
    csv_file << "n,Left_Rectangle_Result,Right_Rectangle_Result,Mid_Rectangle_Result,Trapezoidal_Result,Simpson_Result,"
             << "Left_Rectangle_Time,Right_Rectangle_Time,Mid_Rectangle_Time,Trapezoidal_Time,Simpson_Time,"
             << "Left_Rectangle_Error,Right_Rectangle_Error,Mid_Rectangle_Error,Trapezoidal_Error,Simpson_Error\n";
    
    for (size_t n_idx = 0; n_idx < n_values.size(); n_idx++) {
        csv_file << n_values[n_idx];
        
        for (size_t method_idx = 0; method_idx < methods.size(); method_idx++) {
            csv_file << "," << results[method_idx][n_idx];
        }
        for (size_t method_idx = 0; method_idx < methods.size(); method_idx++) {
            csv_file << "," << times[method_idx][n_idx];
        }
        for (size_t method_idx = 0; method_idx < methods.size(); method_idx++) {
            csv_file << "," << errors[method_idx][n_idx];
        }
        csv_file << "\n";
    }
    csv_file.close();
    
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "РЕЗУЛЬТАТЫ СОХРАНЕНЫ В ФАЙЛ: integration_results_with_graphs.csv\n";
    std::cout << std::string(50, '=') << "\n";
    
    return 0;
}