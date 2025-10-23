#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <string>

// Функция для создания CSV файла
void write_csv_file(const std::vector<std::vector<std::string>>& data, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); i++) {
                file << row[i];
                if (i < row.size() - 1) {
                    file << ",";
                }
            }
            file << "\n";
        }
        file.close();
        std::cout << "✓ Файл создан: " << filename << std::endl;
    } else {
        std::cerr << "✗ Ошибка создания файла: " << filename << std::endl;
    }
}

// Функция counting_sort с сохранением в CSV
std::vector<int> counting_sort_with_csv(const std::vector<int>& arr, const std::string& base_filename) {
    if (arr.empty()) return {};
    
    std::vector<std::vector<std::string>> counting_phase;
    counting_phase.push_back({"Step", "Element", "Value", "Counter_Array", "Description"});
    
    int max_val = *std::max_element(arr.begin(), arr.end());
    std::vector<int> count(max_val + 1, 0);
    
    // Инициализация
    std::string init_counters;
    for (int i = 0; i <= 10; i++) {
        if (i <= max_val) {
            init_counters += std::to_string(i) + ":" + std::to_string(count[i]) + " ";
        }
    }
    if (max_val > 10) init_counters += "...";
    
    counting_phase.push_back({"0", "-", "-", init_counters, "Инициализация массива подсчета"});
    
    // Фаза подсчета
    for (size_t i = 0; i < arr.size(); i++) {
        int num = arr[i];
        count[num]++;
        
        std::string counters;
        int shown = 0;
        for (int j = 0; j <= max_val && shown < 15; j++) {
            if (count[j] > 0) {
                counters += std::to_string(j) + ":" + std::to_string(count[j]) + " ";
                shown++;
            }
        }
        if (shown >= 15) counters += "...";
        
        counting_phase.push_back({
            std::to_string(i + 1),
            "arr[" + std::to_string(i) + "]",
            std::to_string(num),
            counters,
            "Увеличиваем count[" + std::to_string(num) + "] до " + std::to_string(count[num])
        });
    }
    
    write_csv_file(counting_phase, base_filename + "_counting_phase.csv");
    
    // Фаза восстановления
    std::vector<std::vector<std::string>> rebuilding_phase;
    rebuilding_phase.push_back({"Step", "Value", "Remaining", "Sorted_Array", "Action"});
    
    std::vector<int> sorted_arr;
    int step = 1;
    
    for (int i = 0; i <= max_val; i++) {
        if (count[i] > 0) {
            int remaining = count[i];
            while (remaining > 0) {
                sorted_arr.push_back(i);
                remaining--;
                
                std::string sorted_str = "[";
                for (size_t j = 0; j < sorted_arr.size(); j++) {
                    sorted_str += std::to_string(sorted_arr[j]);
                    if (j < sorted_arr.size() - 1) sorted_str += " ";
                }
                sorted_str += "]";
                
                rebuilding_phase.push_back({
                    std::to_string(step++),
                    std::to_string(i),
                    std::to_string(remaining),
                    sorted_str,
                    "Добавлен элемент " + std::to_string(i)
                });
            }
        }
    }
    
    write_csv_file(rebuilding_phase, base_filename + "_rebuilding_phase.csv");
    
    return sorted_arr;
}

// Функция для анализа распределения с сохранением в CSV
void analyze_distribution_csv(const std::vector<int>& arr, const std::string& filename) {
    if (arr.empty()) return;
    
    int max_val = *std::max_element(arr.begin(), arr.end());
    std::vector<int> count(max_val + 1, 0);
    int total = arr.size();
    
    for (int num : arr) {
        count[num]++;
    }
    
    std::vector<std::vector<std::string>> analysis_data;
    analysis_data.push_back({"Value", "Frequency", "Percentage", "Cumulative", "Cumulative_Percentage"});
    
    int cumulative = 0;
    for (int i = 0; i <= max_val; i++) {
        if (count[i] > 0) {
            cumulative += count[i];
            double percentage = static_cast<double>(count[i]) / total * 100;
            double cumulative_percentage = static_cast<double>(cumulative) / total * 100;
            
            std::stringstream percentage_ss, cumulative_ss;
            percentage_ss << std::fixed << std::setprecision(2) << percentage;
            cumulative_ss << std::fixed << std::setprecision(2) << cumulative_percentage;
            
            analysis_data.push_back({
                std::to_string(i),
                std::to_string(count[i]),
                percentage_ss.str(),
                std::to_string(cumulative),
                cumulative_ss.str()
            });
        }
    }
    
    write_csv_file(analysis_data, filename);
}

// Функция для расчета медианы с сохранением в CSV
double calculate_median_csv(const std::vector<int>& sorted_arr, const std::string& filename) {
    if (sorted_arr.empty()) return -1;
    
    std::vector<std::vector<std::string>> median_data;
    median_data.push_back({"Parameter", "Value", "Calculation"});
    
    int n = sorted_arr.size();
    
    std::string array_str = "[";
    for (size_t i = 0; i < sorted_arr.size(); i++) {
        array_str += std::to_string(sorted_arr[i]);
        if (i < sorted_arr.size() - 1) array_str += " ";
    }
    array_str += "]";
    
    median_data.push_back({"Array_Size", std::to_string(n), "n = " + std::to_string(n)});
    median_data.push_back({"Sorted_Array", array_str, "Result of counting sort"});
    
    double median;
    if (n % 2 == 1) {
        int median_index = n / 2;
        median = sorted_arr[median_index];
        median_data.push_back({"Case_Type", "Odd", "n % 2 = 1"});
        median_data.push_back({"Median_Index", std::to_string(median_index), "n / 2 = " + std::to_string(median_index)});
        median_data.push_back({"Median_Value", std::to_string(median), "sorted_array[" + std::to_string(median_index) + "]"});
    } else {
        int median_index1 = n / 2 - 1;
        int median_index2 = n / 2;
        median = (sorted_arr[median_index1] + sorted_arr[median_index2]) / 2.0;
        
        std::stringstream median_ss;
        median_ss << std::fixed << std::setprecision(1) << median;
        
        median_data.push_back({"Case_Type", "Even", "n % 2 = 0"});
        median_data.push_back({"Median_Indices", std::to_string(median_index1) + "," + std::to_string(median_index2), 
                              "n/2-1, n/2"});
        median_data.push_back({"Median_Value", median_ss.str(), 
                              "(" + std::to_string(sorted_arr[median_index1]) + " + " + 
                              std::to_string(sorted_arr[median_index2]) + ") / 2"});
    }
    
    write_csv_file(median_data, filename);
    return median;
}

// Функция для создания итогового отчета
void create_final_report_csv(const std::vector<int>& original, const std::vector<int>& sorted, 
                           double median, const std::string& filename) {
    std::vector<std::vector<std::string>> report;
    report.push_back({"Metric", "Value", "Description"});
    
    report.push_back({"Algorithm", "Counting Sort", "Stable sorting algorithm"});
    report.push_back({"Input_Size", std::to_string(original.size()), "Number of elements"});
    report.push_back({"Value_Range", "0-100", "Product ratings range"});
    
    std::string input_str, output_str;
    for (size_t i = 0; i < original.size(); i++) {
        input_str += std::to_string(original[i]);
        output_str += std::to_string(sorted[i]);
        if (i < original.size() - 1) {
            input_str += " ";
            output_str += " ";
        }
    }
    
    report.push_back({"Input_Data", input_str, "Original array"});
    report.push_back({"Output_Data", output_str, "Sorted array"});
    
    std::stringstream median_ss;
    median_ss << std::fixed << std::setprecision(1) << median;
    report.push_back({"Median_Value", median_ss.str(), "Median product rating"});
    
    int max_val = *std::max_element(original.begin(), original.end());
    report.push_back({"Time_Complexity", "O(n + k)", "n = " + std::to_string(original.size()) + ", k = " + std::to_string(max_val + 1)});
    report.push_back({"Space_Complexity", "O(k)", "k = " + std::to_string(max_val + 1)});
    report.push_back({"Stability", "Yes", "Preserves order of equal elements"});
    
    write_csv_file(report, filename);
}

// Функция для создания HTML файла с графиками
void create_html_with_charts(const std::vector<int>& original, const std::vector<int>& sorted, double median) {
    std::ofstream html_file("counting_sort_analysis.html");
    
    html_file << R"(<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Анализ Counting Sort - Визуализация</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f5f5f5;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        h1, h2 {
            color: #333;
            text-align: center;
        }
        .chart-container {
            position: relative;
            height: 400px;
            margin: 30px 0;
        }
        .table-container {
            overflow-x: auto;
            margin: 20px 0;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 10px 0;
        }
        th, td {
            border: 1px solid #ddd;
            padding: 8px;
            text-align: center;
        }
        th {
            background-color: #f2f2f2;
        }
        tr:nth-child(even) {
            background-color: #f9f9f9;
        }
        .method-info {
            display: flex;
            justify-content: space-around;
            margin: 20px 0;
            flex-wrap: wrap;
        }
        .method-card {
            background: #fff;
            border: 1px solid #ddd;
            border-radius: 5px;
            padding: 15px;
            margin: 10px;
            flex: 1;
            min-width: 200px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .summary {
            background: #e8f4fd;
            border-left: 4px solid #2196F3;
            padding: 15px;
            margin: 20px 0;
            border-radius: 5px;
        }
        .data-section {
            background: #f0f8f0;
            border-left: 4px solid #4CAF50;
            padding: 15px;
            margin: 20px 0;
            border-radius: 5px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Анализ алгоритма Counting Sort</h1>
        
        <div class="summary">
            <h3>Исходные данные</h3>
            <p><strong>Массив:</strong> [)";
    
    // Вставляем исходный массив
    for (size_t i = 0; i < original.size(); i++) {
        html_file << original[i];
        if (i < original.size() - 1) html_file << ", ";
    }
    
    html_file << R"(]</p>
            <p><strong>Размер:</strong> )" << original.size() << R"( элементов</p>
            <p><strong>Диапазон значений:</strong> 0-100</p>
        </div>

        <!-- График распределения частот -->
        <h2>Распределение частот значений</h2>
        <div class="chart-container">
            <canvas id="frequencyChart"></canvas>
        </div>

        <!-- График процесса сортировки -->
        <h2>Процесс Counting Sort</h2>
        <div class="chart-container">
            <canvas id="sortingProcessChart"></canvas>
        </div>

        <!-- График кумулятивного распределения -->
        <h2>Кумулятивное распределение</h2>
        <div class="chart-container">
            <canvas id="cumulativeChart"></canvas>
        </div>

        <!-- Сравнительный график -->
        <h2>Сравнение исходного и отсортированного массива</h2>
        <div class="chart-container">
            <canvas id="comparisonChart"></canvas>
        </div>

        <!-- Таблица с данными -->
        <h2>Данные распределения</h2>
        <div class="table-container">
            <table id="distributionTable">
                <thead>
                    <tr>
                        <th>Значение</th>
                        <th>Частота</th>
                        <th>Процент</th>
                        <th>Кумулятивная частота</th>
                        <th>Кумулятивный процент</th>
                    </tr>
                </thead>
                <tbody id="distributionTableBody">)";

    // Заполняем таблицу данными
    int max_val = *std::max_element(original.begin(), original.end());
    std::vector<int> count(max_val + 1, 0);
    for (int num : original) {
        count[num]++;
    }
    
    int cumulative = 0;
    for (int i = 0; i <= max_val; i++) {
        if (count[i] > 0) {
            cumulative += count[i];
            double percentage = (static_cast<double>(count[i]) / original.size()) * 100;
            double cumulative_percentage = (static_cast<double>(cumulative) / original.size()) * 100;
            
            html_file << "<tr>"
                      << "<td>" << i << "</td>"
                      << "<td>" << count[i] << "</td>"
                      << "<td>" << std::fixed << std::setprecision(2) << percentage << "%</td>"
                      << "<td>" << cumulative << "</td>"
                      << "<td>" << std::fixed << std::setprecision(2) << cumulative_percentage << "%</td>"
                      << "</tr>";
        }
    }

    html_file << R"(
                </tbody>
            </table>
        </div>

        <!-- Информация о методе -->
        <h2>Характеристики Counting Sort</h2>
        <div class="method-info">
            <div class="method-card">
                <h4>Сложность времени</h4>
                <p><strong>O(n + k)</strong></p>
                <p>n = )" << original.size() << ", k = " << (max_val + 1) << R"(</p>
                <p>O()" << original.size() << " + " << (max_val + 1) << ") = O(" << (original.size() + max_val + 1) << R"()</p>
            </div>
            <div class="method-card">
                <h4>Сложность памяти</h4>
                <p><strong>O(k)</strong></p>
                <p>k = )" << (max_val + 1) << R"(</p>
                <p>Требуется массив размером )" << (max_val + 1) << R"(</p>
            </div>
            <div class="method-card">
                <h4>Стабильность</h4>
                <p><strong>Да</strong></p>
                <p>Сохраняет порядок равных элементов</p>
            </div>
            <div class="method-card">
                <h4>Применимость</h4>
                <p><strong>Целые числа</strong></p>
                <p>Малый диапазон значений</p>
                <p>Неотрицательные числа</p>
            </div>
        </div>

        <!-- Расчет медианы -->
        <div class="data-section">
            <h3>Расчет медианы</h3>
            <p><strong>Отсортированный массив:</strong> [)";
    
    // Вставляем отсортированный массив
    for (size_t i = 0; i < sorted.size(); i++) {
        html_file << sorted[i];
        if (i < sorted.size() - 1) html_file << ", ";
    }
    
    html_file << R"(]</p>
            <p><strong>Размер массива:</strong> )" << sorted.size();
    
    if (sorted.size() % 2 == 1) {
        int median_index = sorted.size() / 2;
        html_file << " (нечетное)</p>"
                  << "<p><strong>Медианный индекс:</strong> " << median_index << " (n/2 = " << sorted.size() << "/2 = " << median_index << ")</p>"
                  << "<p><strong>Медиана:</strong> sorted_array[" << median_index << "] = " << median << "</p>";
    } else {
        int median_index1 = sorted.size() / 2 - 1;
        int median_index2 = sorted.size() / 2;
        html_file << " (четное)</p>"
                  << "<p><strong>Медианные индексы:</strong> " << median_index1 << " и " << median_index2 << " (n/2-1 и n/2)</p>"
                  << "<p><strong>Медиана:</strong> (sorted_array[" << median_index1 << "] + sorted_array[" << median_index2 << "]) / 2 = "
                  << "(" << sorted[median_index1] << " + " << sorted[median_index2] << ") / 2 = " << median << "</p>";
    }
    
    html_file << R"(
        </div>
    </div>

    <script>
        // Данные из C++ программы
        const originalArray = [)";
    
    for (size_t i = 0; i < original.size(); i++) {
        html_file << original[i];
        if (i < original.size() - 1) html_file << ", ";
    }
    
    html_file << R"(];
        const sortedArray = [)";
    
    for (size_t i = 0; i < sorted.size(); i++) {
        html_file << sorted[i];
        if (i < sorted.size() - 1) html_file << ", ";
    }
    
    html_file << R"(];
        
        // Расчет распределения частот
        function calculateFrequencyDistribution(arr) {
            const frequency = {};
            arr.forEach(num => {
                frequency[num] = (frequency[num] || 0) + 1;
            });
            return frequency;
        }

        const frequencyData = calculateFrequencyDistribution(originalArray);
        const values = Object.keys(frequencyData).map(Number).sort((a, b) => a - b);
        const frequencies = values.map(val => frequencyData[val]);

        // Расчет кумулятивного распределения
        let cumulative = 0;
        const cumulativeData = values.map((val, index) => {
            cumulative += frequencyData[val];
            return cumulative;
        });

        const percentages = values.map(val => 
            ((frequencyData[val] / originalArray.length) * 100).toFixed(1)
        );

        const cumulativePercentages = cumulativeData.map(cum => 
            ((cum / originalArray.length) * 100).toFixed(1)
        );

        // График распределения частот
        const freqCtx = document.getElementById('frequencyChart').getContext('2d');
        new Chart(freqCtx, {
            type: 'bar',
            data: {
                labels: values,
                datasets: [{
                    label: 'Частота',
                    data: frequencies,
                    backgroundColor: 'rgba(54, 162, 235, 0.8)',
                    borderColor: 'rgba(54, 162, 235, 1)',
                    borderWidth: 1
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Частота'
                        }
                    },
                    x: {
                        title: {
                            display: true,
                            text: 'Значения'
                        }
                    }
                }
            }
        });

        // График процесса сортировки
        const sortCtx = document.getElementById('sortingProcessChart').getContext('2d');
        new Chart(sortCtx, {
            type: 'line',
            data: {
                labels: Array.from({length: originalArray.length}, (_, i) => i + 1),
                datasets: [
                    {
                        label: 'Исходный массив',
                        data: originalArray,
                        borderColor: 'rgba(255, 99, 132, 1)',
                        backgroundColor: 'rgba(255, 99, 132, 0.2)',
                        tension: 0.1,
                        pointRadius: 6
                    },
                    {
                        label: 'Отсортированный массив',
                        data: sortedArray,
                        borderColor: 'rgba(75, 192, 192, 1)',
                        backgroundColor: 'rgba(75, 192, 192, 0.2)',
                        tension: 0.1,
                        pointRadius: 6
                    }
                ]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: {
                        title: {
                            display: true,
                            text: 'Значения'
                        }
                    },
                    x: {
                        title: {
                            display: true,
                            text: 'Позиция в массиве'
                        }
                    }
                }
            }
        });

        // График кумулятивного распределения
        const cumCtx = document.getElementById('cumulativeChart').getContext('2d');
        new Chart(cumCtx, {
            type: 'line',
            data: {
                labels: values,
                datasets: [
                    {
                        label: 'Кумулятивная частота',
                        data: cumulativeData,
                        borderColor: 'rgba(153, 102, 255, 1)',
                        backgroundColor: 'rgba(153, 102, 255, 0.2)',
                        yAxisID: 'y',
                        tension: 0.4
                    },
                    {
                        label: 'Кумулятивный процент',
                        data: cumulativePercentages,
                        borderColor: 'rgba(255, 159, 64, 1)',
                        backgroundColor: 'rgba(255, 159, 64, 0.2)',
                        yAxisID: 'y1',
                        tension: 0.4
                    }
                ]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: {
                        type: 'linear',
                        display: true,
                        position: 'left',
                        title: {
                            display: true,
                            text: 'Кумулятивная частота'
                        }
                    },
                    y1: {
                        type: 'linear',
                        display: true,
                        position: 'right',
                        title: {
                            display: true,
                            text: 'Кумулятивный процент (%)'
                        },
                        grid: {
                            drawOnChartArea: false
                        },
                        max: 100
                    },
                    x: {
                        title: {
                            display: true,
                            text: 'Значения'
                        }
                    }
                }
            }
        });

        // Сравнительный график
        const compCtx = document.getElementById('comparisonChart').getContext('2d');
        new Chart(compCtx, {
            type: 'bar',
            data: {
                labels: Array.from({length: originalArray.length}, (_, i) => `Элемент ${i + 1}`),
                datasets: [
                    {
                        label: 'Исходный массив',
                        data: originalArray,
                        backgroundColor: 'rgba(255, 99, 132, 0.8)'
                    },
                    {
                        label: 'Отсортированный массив',
                        data: sortedArray,
                        backgroundColor: 'rgba(75, 192, 192, 0.8)'
                    }
                ]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Значения'
                        }
                    }
                }
            }
        });
    </script>
</body>
</html>)";

    html_file.close();
    std::cout << "✓ HTML файл с графиками создан: counting_sort_analysis.html" << std::endl;
}

// Функция для красивого вывода массива в консоль
void print_array(const std::vector<int>& arr, const std::string& title = "") {
    if (!title.empty()) {
        std::cout << title;
    }
    std::cout << "[";
    for (size_t i = 0; i < arr.size(); i++) {
        std::cout << arr[i];
        if (i < arr.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

// Функция для вывода медианы с подробным объяснением
void print_median_explanation(const std::vector<int>& sorted_arr, double median) {
    int n = sorted_arr.size();
    std::cout << "📊 РАСЧЕТ МЕДИАНЫ:" << std::endl;
    std::cout << "   Размер массива: " << n << " элементов" << std::endl;
    
    if (n % 2 == 1) {
        int median_index = n / 2;
        std::cout << "   Нечетное количество элементов" << std::endl;
        std::cout << "   Медианный индекс: " << median_index << " (n / 2)" << std::endl;
        std::cout << "   Медиана = sorted_arr[" << median_index << "] = " << median << std::endl;
    } else {
        int median_index1 = n / 2 - 1;
        int median_index2 = n / 2;
        std::cout << "   Четное количество элементов" << std::endl;
        std::cout << "   Медианные индексы: " << median_index1 << " и " << median_index2 << " (n/2-1 и n/2)" << std::endl;
        std::cout << "   Медиана = (sorted_arr[" << median_index1 << "] + sorted_arr[" << median_index2 << "]) / 2" << std::endl;
        std::cout << "   Медиана = (" << sorted_arr[median_index1] << " + " << sorted_arr[median_index2] << ") / 2 = " << median << std::endl;
    }
}

// Основная функция
int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "   COUNTING SORT - ПОЛНЫЙ АНАЛИЗ" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;
    
    std::vector<int> ratings = {5, 28, 14, 73, 42, 61, 33, 91, 22, 55, 19};
    
    // Вывод исходных данных
    std::cout << "🎯 ИСХОДНЫЕ ДАННЫЕ:" << std::endl;
    print_array(ratings, "   Исходный массив: ");
    std::cout << std::endl;
    
    // 1. Сортировка с сохранением этапов в CSV
    std::cout << "1. СОРТИРОВКА COUNTING SORT..." << std::endl;
    std::vector<int> sorted = counting_sort_with_csv(ratings, "counting_sort");
    
    // Вывод отсортированного массива
    std::cout << std::endl;
    std::cout << "✅ РЕЗУЛЬТАТЫ СОРТИРОВКИ:" << std::endl;
    print_array(sorted, "   Отсортированный массив: ");
    std::cout << std::endl;
    
    // 2. Анализ распределения
    std::cout << "2. АНАЛИЗ РАСПРЕДЕЛЕНИЯ..." << std::endl;
    analyze_distribution_csv(ratings, "distribution_analysis.csv");
    
    // 3. Расчет медианы
    std::cout << "3. РАСЧЕТ МЕДИАНЫ..." << std::endl;
    double median = calculate_median_csv(sorted, "median_calculation.csv");
    
    // Вывод медианы с объяснением
    std::cout << std::endl;
    std::cout << "🎯 РЕЗУЛЬТАТ РАСЧЕТА МЕДИАНЫ:" << std::endl;
    print_median_explanation(sorted, median);
    std::cout << std::endl;
    
    // 4. Итоговый отчет
    std::cout << "4. СОЗДАНИЕ ИТОГОВОГО ОТЧЕТА..." << std::endl;
    create_final_report_csv(ratings, sorted, median, "final_report.csv");
    
    // 5. Создание HTML файла с графиками
    std::cout << "5. СОЗДАНИЕ HTML ФАЙЛА С ГРАФИКАМИ..." << std::endl;
    create_html_with_charts(ratings, sorted, median);
    
    std::cout << std::endl;
    std::cout << "🎉 ВСЕ ФАЙЛЫ УСПЕШНО СОЗДАНЫ!" << std::endl;
    std::cout << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "📁 СОЗДАННЫЕ ФАЙЛЫ:" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "• counting_sort_counting_phase.csv   - Фаза подсчета" << std::endl;
    std::cout << "• counting_sort_rebuilding_phase.csv - Фаза восстановления" << std::endl;
    std::cout << "• distribution_analysis.csv          - Анализ распределения" << std::endl;
    std::cout << "• median_calculation.csv             - Расчет медианы" << std::endl;
    std::cout << "• final_report.csv                   - Итоговый отчет" << std::endl;
    std::cout << "• counting_sort_analysis.html        - HTML с графиками" << std::endl;
    std::cout << std::endl;
    std::cout << "💡 Откройте counting_sort_analysis.html в браузере для просмотра графиков!" << std::endl;
    std::cout << std::endl;
    
    return 0;
}