#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <map>
#include <chrono>
#include <future>

std::map<std::pair<size_t, size_t>, size_t> hint_buffer;
const size_t code_digits = 4;
const int is0A0B = 0;
const int is0A1B = 1;
const int is0A2B = 2;
const int is0A3B = 3;
const int is0A4B = 4;
const int is1A0B = 5;
const int is1A1B = 6;
const int is1A2B = 7;
const int is1A3B = 8;
const int is2A0B = 9;
const int is2A1B = 10;
const int is2A2B = 11;
const int is3A0B = 12;
const int is4A0B = 13;

template <class CharSetType>
std::ostream &operator<<(std::ostream &output, const std::vector<CharSetType> vector)
{
    output << vector.at(code_digits) << " ";
    for (int i = 0; i < code_digits; i++)
        output << vector.at(i);
    return output;
}

template <class CharSetType>
std::istream &operator>>(std::istream &input, std::vector<CharSetType> &vector)
{
    std::string buffer;
    input >> buffer;
    for (auto &i : buffer)
        vector.push_back(i - '0');
    return input;
}

template <class CharSetType, size_t char_set_size>
void GeneratePossibleAnswers(size_t pos, size_t depth, const CharSetType (&char_set)[char_set_size], const size_t code_digits, std::vector<std::vector<CharSetType>> &possible_set, std::vector<CharSetType> &possible_set_element)
{

    if (depth == code_digits)
    {
        do
            possible_set.push_back(possible_set_element);
        while (next_permutation(possible_set_element.begin(), possible_set_element.end()));
        // Arrangement after Combination then put it into possible_set
        return;
    }
    for (size_t i = pos; i < char_set_size; i++)
    {
        possible_set_element.at(depth) = char_set[i];
        GeneratePossibleAnswers(i + 1, depth + 1, char_set, code_digits, possible_set, possible_set_element);
        // Combination by DFS
    }
}

template <class CharSetType, size_t char_set_size>
void GetPossibleAnswers(const CharSetType (&char_set)[char_set_size], const size_t code_digits, std::vector<std::vector<CharSetType>> &possible_set)
{
    std::vector<CharSetType> possible_set_element(code_digits, 0);
    GeneratePossibleAnswers(0, 0, char_set, code_digits, possible_set, possible_set_element);
    for (int i = 0; i < possible_set.size(); i++)
        possible_set.at(i).push_back(i);
}

template <class CharSetType>
size_t GetHint(std::vector<CharSetType> secret, std::vector<CharSetType> guess)
{
    auto secret_guess = std::make_pair(secret.at(code_digits), guess.at(code_digits));
    auto guess_secret = std::make_pair(guess.at(code_digits), secret.at(code_digits));

    if (secret.at(code_digits) <= guess.at(code_digits))
        return hint_buffer[secret_guess];
    if (secret.at(code_digits) > guess.at(code_digits))
        return hint_buffer[guess_secret];
    return -1;
}

template <class CharSetType>
size_t GetHintCalc(std::vector<CharSetType> secret, std::vector<CharSetType> guess)
{
    size_t bulls = 0, cows = 0;
    size_t buffer[10] = {0};

    for (size_t i = 0; i < code_digits; i++)
    {
        if (secret.at(i) == guess.at(i))
            bulls++;
        else
        {
            // TODO: Support Char Type
            if (buffer[secret.at(i)]++ < 0)
                cows++;
            if (buffer[guess.at(i)]-- > 0)
                cows++;
        }
    }

    if (bulls == 2)
        return 9 + cows;
    if (bulls == 3)
        return 12;
    if (bulls == 4)
        return 13;
    return bulls * 5 + cows;
}

template <class CharSetType>
void GenerateHintBuffer(size_t pos, size_t depth, const size_t group_size, const std::vector<std::vector<CharSetType>> possible_set, std::vector<std::vector<CharSetType>> &secret_guess)
{
    if (depth == group_size)
    {
        hint_buffer[std::make_pair(secret_guess.at(0).at(code_digits), secret_guess.at(1).at(code_digits))] = GetHintCalc(secret_guess.at(0), secret_guess.at(1));
        std::cout << secret_guess.at(0) << " " << secret_guess.at(1) << " " << hint_buffer[std::make_pair(secret_guess.at(0).at(code_digits), secret_guess.at(1).at(code_digits))] << std::endl;
        return;
    }
    for (size_t i = pos; i < possible_set.size(); i++)
    {
        secret_guess.at(depth) = possible_set.at(i);
        GenerateHintBuffer(i + 1, depth + 1, group_size, possible_set, secret_guess);
        // Combination by DFS
    }
}

template <class CharSetType>
std::vector<CharSetType> CalculateBestQuestion(const std::vector<std::vector<CharSetType>> possible_set_modified)
{
    size_t best_question_index = 0;
    size_t min_group_calc_best_classification = 5040;

    for (size_t i = 0; i < possible_set_modified.size(); i++)
    {
        size_t hint_groups[14] = {0};
        size_t max_group = 0;
        for (size_t j = 0; j < possible_set_modified.size(); j++)
        {
            size_t temp = ++hint_groups[GetHint(possible_set_modified.at(i), possible_set_modified.at(j))];
            if (temp > min_group_calc_best_classification)
                break;
        }
        max_group = *std::max_element(hint_groups, hint_groups + 14);
        if (max_group < min_group_calc_best_classification)
        {
            min_group_calc_best_classification = max_group;
            best_question_index = i;
        }
    }
    return possible_set_modified.at(best_question_index);
}

template <class CharSetType>
void ExcludeAnswers(std::vector<CharSetType> guess, size_t hint, std::vector<std::vector<CharSetType>> &possible_set_modified)
{
    for (size_t i = 0; i < possible_set_modified.size();)
    {
        if (GetHint(possible_set_modified.at(i), guess) != hint)
            possible_set_modified.erase(possible_set_modified.begin() + i);
        else
            i++;
    }
}

size_t *Compute1(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 0; i < 500; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute2(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 500; i < 1000; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute3(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 1000; i < 1500; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute4(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 1500; i < 2000; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute5(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 2000; i < 2500; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute6(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 2500; i < 3000; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute7(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 3000; i < 3500; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute8(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 3500; i < 4000; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute9(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 4000; i < 4500; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

size_t *Compute10(std::vector<std::vector<size_t>> possible_set)
{
    static size_t guess_times_arr[100] = {0};
    for (size_t i = 4500; i < 5040; i++)
    {
        std::vector<std::vector<size_t>> possible_set_modified(possible_set);
        std::vector<size_t> secret(possible_set.at(i));
        std::vector<size_t> guess({0, 1, 2, 3, 0});

        bool not_found = true;
        size_t hint, guess_times = 0;

        hint = GetHint(secret, guess);
        guess_times++;
        if (hint == is4A0B)
            not_found = false;
        else
            ExcludeAnswers(guess, hint, possible_set_modified);

        while (not_found)
        {
            guess = CalculateBestQuestion(possible_set_modified);
            hint = GetHint(secret, guess);
            guess_times++;
            if (hint == is4A0B)
            {
                not_found = false;
                break;
            }
            ExcludeAnswers(guess, hint, possible_set_modified);
        }
        std::cout << secret << " Hits!" << std::endl;
        guess_times_arr[guess_times]++;
    }
    return guess_times_arr;
}

int main()
{
    const size_t char_set[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::vector<std::vector<size_t>> possible_set;
    std::vector<std::vector<size_t>> secret_guess(2);

    size_t *result1_ptr;
    size_t *result2_ptr;
    size_t *result3_ptr;
    size_t *result4_ptr;
    size_t *result5_ptr;
    size_t *result6_ptr;
    size_t *result7_ptr;
    size_t *result8_ptr;
    size_t *result9_ptr;
    size_t *result10_ptr;

    GetPossibleAnswers(char_set, code_digits, possible_set);
    GenerateHintBuffer(0, 0, 2, possible_set, secret_guess);
    std::future<size_t *> result1 = std::async(std::launch::async, Compute1, possible_set),
                          result2 = std::async(std::launch::async, Compute2, possible_set),
                          result3 = std::async(std::launch::async, Compute3, possible_set),
                          result4 = std::async(std::launch::async, Compute4, possible_set),
                          result5 = std::async(std::launch::async, Compute5, possible_set),
                          result6 = std::async(std::launch::async, Compute6, possible_set),
                          result7 = std::async(std::launch::async, Compute7, possible_set),
                          result8 = std::async(std::launch::async, Compute8, possible_set),
                          result9 = std::async(std::launch::async, Compute9, possible_set),
                          result10 = std::async(std::launch::async, Compute10, possible_set);

    result1_ptr = result1.get();
    result2_ptr = result2.get();
    result3_ptr = result3.get();
    result4_ptr = result4.get();
    result5_ptr = result5.get();
    result6_ptr = result6.get();
    result7_ptr = result7.get();
    result8_ptr = result8.get();
    result9_ptr = result9.get();
    result10_ptr = result10.get();

    for (size_t i = 0; i < 100; i++)
    {
        std::cout << i << " Times: " << result1_ptr[i] + result2_ptr[i] + result3_ptr[i] + result4_ptr[i] + result5_ptr[i] + result6_ptr[i] + result7_ptr[i] + result8_ptr[i] + result9_ptr[i] + result10_ptr[i] << std::endl;
    }
    size_t sum = 0;
    for (size_t i = 0; i < 100; i++)
    {
        sum += result1_ptr[i] * i;
        sum += result2_ptr[i] * i;
        sum += result3_ptr[i] * i;
        sum += result4_ptr[i] * i;
        sum += result5_ptr[i] * i;
        sum += result6_ptr[i] * i;
        sum += result7_ptr[i] * i;
        sum += result8_ptr[i] * i;
        sum += result9_ptr[i] * i;
        sum += result10_ptr[i] * i;
    }
    std::cout << "Average Times: " << sum / 5040.0 << std::endl;
    system("pause");
    return 0;
}
