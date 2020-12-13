/*
 * Студент: Корицкий Александр
 * Вариант: 12
 * Формулировка задания
 * Задача о больнице. В больнице два врача принимают пациентов,
 * выслушивают их жалобы и отправляют их или к стоматологу или к хирургу
 * или к терапевту. Стоматолог, хирург и терапевт лечат пациента. Каждый
 * врач может принять только одного пациента за раз. Пациенты стоят в
 * очереди к врачам и никогда их не покидают. Создать многопоточное
 * приложение, моделирующее рабочий день клиники
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

/// Вектор для результата
std::vector<std::string> results;

/// Объявление переменных для программы (буфер, индекс для чтения, для записи и кол-во несвободных эл-тов буфера
const int bufSize = 2;
int buf[bufSize];
int readIndex = 0, writeIndex = 0, countNotEmptyEl = 0;

/// Мьютексы
pthread_mutex_t mutex;
pthread_cond_t notFull;
pthread_cond_t notEmpty;

/// Начальная функция для потоков-врачей (писатели)
void *startDoctorProcess(void *param) {
    int data;
    while (true) {
        data = rand() % 3 + 1;
        pthread_mutex_lock(&mutex);

        while (countNotEmptyEl == bufSize)
            pthread_cond_wait(&notFull, &mutex);

        /// Записываем созданный элемент в буфер
        buf[writeIndex] = data;
        writeIndex = (writeIndex + 1) % bufSize;
        countNotEmptyEl++;

        pthread_mutex_unlock(&mutex);

        /// Вызываем потоки - читателей
        pthread_cond_broadcast(&notEmpty);
        sleep(rand() % 2 + 0.02);
    }
    return nullptr;
}

/// Начальная функция для потоков-пациентов (читателей)
void *startPatientProcess(void *param) {
    int number = *((int *) param);
    int result;

    /// Берем элемент из буфера
    pthread_mutex_lock(&mutex);

    /// Проверяем возможность "засыпания" потока
    while (countNotEmptyEl == 0)
        pthread_cond_wait(&notEmpty, &mutex);

    /// Берем из буфера
    result = buf[readIndex];
    readIndex = (readIndex + 1) % bufSize;

    /// Ячейка становится свободной
    countNotEmptyEl--;
    results.push_back(std::to_string(number) + " " + std::to_string(result) + " " + std::to_string(readIndex));
    pthread_mutex_unlock(&mutex);

    /// "Будим" потоки-писателей
    pthread_cond_broadcast(&notFull);
    return nullptr;
}

int main() {
    /// Локализация русского языка
    setlocale(LC_ALL, "Russian");

    /// Основная программа
    /// Начинаем с ввода количества пациентов в больнице
    std::string howMuch;
    std::cout << ("Введите кол-во пациентов сегодня в больнице: ");
    std::cin >> howMuch;

    /// Проверяем входные данные по паттерну цифр
    size_t input = howMuch.find_first_not_of("0123456789");
    while (input != std::string::npos || stoi(howMuch) == 0 || stoi(howMuch) > 1000) {
        std::cout << "Неправильный ввод! Пожалуйста, попробуйте еще раз: ";
        std::cin >> howMuch;
        input = howMuch.find_first_not_of("0123456789");
    }

    int countPatients = stoi(howMuch);

    /// Инициализируем мьютексы
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&notFull, nullptr);
    pthread_cond_init(&notEmpty, nullptr);

    int mainIndex;

    /// Запускаем поток-врачи
    pthread_t firstThread[2];
    int fThread[2];
    for (mainIndex = 0; mainIndex < 2; mainIndex++) {
        fThread[mainIndex] = mainIndex + 1;
        pthread_create(&firstThread[mainIndex], nullptr, startDoctorProcess, (void *) (fThread + mainIndex));
    }

    /// Запускаем поток-пациентов
    auto *secondThreat = new pthread_t[countPatients];
    int *sThread = new int[countPatients];
    for (mainIndex = 0; mainIndex < countPatients; mainIndex++) {
        sThread[mainIndex] = mainIndex + 1;
        pthread_create(&secondThreat[mainIndex], nullptr, startPatientProcess, (void *) (sThread + mainIndex));
    }
    for (mainIndex = 0; mainIndex < countPatients; mainIndex++) {
        sThread[mainIndex] = mainIndex + 1;
        pthread_join(secondThreat[mainIndex], nullptr);
    }

    /// Объявляем вектора
    std::vector<int> patients;
    std::vector<std::string> decision;
    std::vector<int> numberOfDoctor;

    for (auto patient: results) {
        std::string words[3];
        int index = -1;

        for (int i = 0; i < patient.size(); i++) {
            if (patient[i] == ' ') {
                words[++index] = patient.substr(0, i);
                patient = patient.substr(i + 1, results.size() - i - 1);
                i = 0;
            }
        }

        if (words[2].empty()) {
            words[2] = patient;
        }

        patients.push_back(stoi(words[0]));
        decision.push_back(words[1]);
        numberOfDoctor.push_back(stoi(words[2]));
    }

    /// Временные переменные для обмена элементов
    int tmp1;
    std::string tmp2;
    int tmp3;

    /// Обычная сортировка пузырьком
    for (int i = 0; i < results.size() - 1; i++) {
        for (int j = 0; j < results.size() - i - 1; j++) {
            if (patients[j] > patients[j + 1]) {
                /// Используем наши временные переменные для сортировки
                tmp1 = patients[j];
                patients[j] = patients[j + 1];
                patients[j + 1] = tmp1;

                tmp2 = decision[j];
                decision[j] = decision[j + 1];
                decision[j + 1] = tmp2;

                tmp3 = numberOfDoctor[j];
                numberOfDoctor[j] = numberOfDoctor[j + 1];
                numberOfDoctor[j + 1] = tmp3;
            }
        }
    }

    /// Определяем, какой врач будет принимать
    for (int i = 0; i < results.size(); i++) {
        std::string doctor;

        if (stoi(decision[i]) == 1)
            doctor = "стоматологу";
        else if (stoi(decision[i]) == 2)
            doctor = "хирургу";
        else
            doctor = "терапевту";

        std::cout << "Пациент #" << patients[i] << " идет к " << doctor << "; доктор - "
            << numberOfDoctor[i] << std::endl;
    }

    /// Удаляем поток
    delete[] secondThreat;
    delete[] sThread;

    return 0;
}