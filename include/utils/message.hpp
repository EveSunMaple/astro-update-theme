#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

// MessagePrinter 类，用于打印消息
class MessagePrinter
{
private:
    std::unordered_map<std::string, std::string> messages;

    // 辅助函数：将单个参数转换为字符串
    template <typename T>
    std::string stringify(const T &arg) const
    {
        std::ostringstream oss;
        oss << arg;
        return oss.str();
    }

    /**
     * 替换字符串中的占位符
     *
     * 这个函数会遍历 `args` 数组，将 `message` 中的占位符（格式为 `<n>`）替换为 `args` 中的字符串。
     * 占位符 `<n>` 中的 `n` 表示 `args` 数组的索引。
     *
     * @param message 源字符串，包含占位符
     * @param args 字符串向量，用于替换占位符
     * @return 替换占位符后的新字符串
     */
    std::string replacePlaceholders(const std::string &message, const std::vector<std::string> &args) const
    {
        std::string result = message;
        for (size_t i = 0; i < args.size(); ++i)
        {
            std::string placeholder = "<" + std::to_string(i) + ">";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos)
            {
                result.replace(pos, placeholder.length(), args[i]);
                pos += args[i].length();
            }
        }
        return result;
    }

public:
    // 构造函数：加载JSON文件
    MessagePrinter(const std::string &jsonFile)
    {
        std::ifstream file(jsonFile);
        if (!file)
        {
            std::cerr << "Unable to open JSON file: " << jsonFile << std::endl;
            return;
        }
        nlohmann::json jsonData;
        try
        {
            file >> jsonData;
            for (auto &element : jsonData.items())
            {
                messages[element.key()] = element.value();
            }
        }
        catch (const nlohmann::json::parse_error &e)
        {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }
    }

    // 原始的print函数，仅接受消息ID
    void print(const std::string &messageId) const
    {
        auto it = messages.find(messageId);
        if (it != messages.end())
        {
            std::cout << it->second << std::endl;
        }
        else
        {
            std::cerr << "Message ID not found: " << messageId << std::endl;
        }
    }

    /**
     * 打印具有给定消息 ID 的格式化消息
     *
     * 此函数接受消息 ID 字符串以及任意数量的参数。它使用 replacePlaceholders 函数来替换消息中的占位符。
     *
     * @param messageId 要打印的消息的标识符
     * @param args 用于替换占位符的参数包
     *
     * @throws 如果找不到消息 ID，则会输出错误消息到标准错误流
     */
    template <typename... Args>
    void print(const std::string &messageId, Args &&...args) const
    {
        auto it = messages.find(messageId);
        if (it != messages.end())
        {
            // 将每个参数转换为字符串并存入vector
            std::vector<std::string> argVec = {stringify(std::forward<Args>(args))...};
            // 替换占位符
            std::string formattedMessage = replacePlaceholders(it->second, argVec);
            std::cout << formattedMessage << std::endl;
        }
        else
        {
            std::cerr << "Message ID not found: " << messageId << std::endl;
        }
    }
};

#endif
