#include "IconvHelper.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

// 外部声明的 GBK 测试字符串
extern char const* gbk_str;

// 颜色输出辅助函数
void printSuccess(const std::string& msg) {
    std::cout << "\033[32m\xE2\x9C\x93 " << msg << "\033[0m" << std::endl;
}

void printError(const std::string& msg) {
    std::cout << "\033[31m\xE2\x9C\x97 " << msg << "\033[0m" << std::endl;
}

void printInfo(const std::string& msg) {
    std::cout << "\033[34m\xE2\x9D\x80 " << msg << "\033[0m" << std::endl;
}

void printTestHeader(const std::string& testName) {
    std::cout << "\n\033[33m=== " << testName << " ===\033[0m" << std::endl;
}

// 测试结果辅助函数
void checkResult(const IConvResult& result, const std::string& testName) {
    if (result.errorNumber == 0) {
        printSuccess(testName + " - 转换成功");
        printInfo("结果长度: " + std::to_string(result.result.length()) + " 字节");
        if (!result.result.empty()) {
            printInfo("结果前20字节 (hex): ");
            for (size_t i = 0; i < std::min(size_t(20), result.result.length()); ++i) {
                printf("%02x ", (unsigned char)result.result[i]);
            }
            std::cout << std::endl;
        }
    } else {
        printError(testName + " - 转换失败");
        printError("错误号: " + std::to_string(result.errorNumber));
        printError("错误信息: " + result.errorMessage);
    }
}

// 测试1: 静态方法转换功能
void testStaticConvert() {
    printTestHeader("测试静态转换方法");
    
    // UTF-8 测试字符串
    std::string utf8_str = "Hello 世界! 这是一个UTF-8字符串。";
    
    // 1.1 UTF-8 转 GBK
    auto result1 = IconvHelper::Convert(utf8_str, "UTF-8", "GBK");
    checkResult(result1, "UTF-8 -> GBK");
    
    // 1.2 GBK 转 UTF-8 (使用外部 GBK 字符串)
    auto result2 = IconvHelper::Convert(gbk_str, "GBK", "UTF-8");
    checkResult(result2, "GBK -> UTF-8");
    
    // 1.3 UTF-8 转 UTF-16LE
    auto result3 = IconvHelper::Convert(utf8_str, "UTF-8", "UTF-16LE");
    checkResult(result3, "UTF-8 -> UTF-16LE");
    
    // 1.4 UTF-8 转 UTF-32
    auto result4 = IconvHelper::Convert(utf8_str, "UTF-8", "UTF-32");
    checkResult(result4, "UTF-8 -> UTF-32");
    
    // 1.5 空字符串转换
    auto result5 = IconvHelper::Convert("", "UTF-8", "GBK");
    checkResult(result5, "空字符串转换");
    
    // 1.6 无效编码测试
    auto result6 = IconvHelper::Convert(utf8_str, "INVALID_ENCODING", "UTF-8");
    checkResult(result6, "无效源编码测试");
    
    auto result7 = IconvHelper::Convert(utf8_str, "UTF-8", "INVALID_ENCODING");
    checkResult(result7, "无效目标编码测试");
}

// 测试2: 构造函数和析构函数
void testConstructorDestructor() {
    printTestHeader("测试构造函数和析构函数");
    
    try {
        // 2.1 正常构造
        {
            IconvHelper helper("UTF-8", "GBK");
            printSuccess("正常构造 UTF-8->GBK");
        }
        printSuccess("析构成功");
        
        // 2.2 无效编码构造（应该抛异常）
        try {
            IconvHelper helper("INVALID", "UTF-8");
            printError("无效编码构造应该抛异常，但没有抛出");
        } catch (const std::exception& e) {
            printSuccess("无效编码构造正确抛出异常: " + std::string(e.what()));
        }
        
    } catch (const std::exception& e) {
        printError("构造函数测试失败: " + std::string(e.what()));
    }
}

// 测试3: 移动语义
void testMoveSemantics() {
    printTestHeader("测试移动语义");
    
    try {
        // 3.1 移动构造
        IconvHelper helper1("UTF-8", "GBK");
        IconvHelper helper2 = std::move(helper1);
        printSuccess("移动构造成功");
        
        // 3.2 移动赋值
        IconvHelper helper3("GBK", "UTF-8");
        helper3 = std::move(helper2);
        printSuccess("移动赋值成功");
        
    } catch (const std::exception& e) {
        printError("移动语义测试失败: " + std::string(e.what()));
    }
}

// 测试4: 实例方法转换功能
void testInstanceConvert() {
    printTestHeader("测试实例方法转换");
    
    try {
        // 4.1 UTF-8 -> GBK 转换器
        IconvHelper utf8ToGbk("UTF-8", "GBK");
        std::string utf8_str = "Hello 世界! 实例方法测试。";
        auto result1 = utf8ToGbk.Convert(utf8_str);
        checkResult(result1, "实例方法 UTF-8 -> GBK");
        
        // 4.2 GBK -> UTF-8 转换器
        IconvHelper gbkToUtf8("GBK", "UTF-8");
        auto result2 = gbkToUtf8.Convert(gbk_str);
        checkResult(result2, "实例方法 GBK -> UTF-8");
        
        // 4.3 多次转换测试（复用同一个实例）
        std::vector<std::string> testStrings = {
            "第一次转换",
            "第二次转换 with English",
            "第三次转换 123!@#",
            "第四次转换 特殊字符：©®™"
        };
        
        for (size_t i = 0; i < testStrings.size(); ++i) {
            auto result = utf8ToGbk.Convert(testStrings[i]);
            checkResult(result, "多次转换测试 #" + std::to_string(i + 1));
        }
        
    } catch (const std::exception& e) {
        printError("实例方法测试失败: " + std::string(e.what()));
    }
}

// 测试5: 边界情况和错误处理
void testEdgeCases() {
    printTestHeader("测试边界情况和错误处理");
    
    try {
        IconvHelper helper("UTF-8", "ASCII");
        
        // 5.1 包含非ASCII字符的UTF-8字符串转ASCII（应该失败或截断）
        std::string utf8WithChinese = "Hello 世界";
        auto result1 = helper.Convert(utf8WithChinese);
        checkResult(result1, "UTF-8(含中文) -> ASCII");
        
        // 5.2 超长字符串
        std::string longString(10000, 'A');
        longString += "测试";
        auto result2 = helper.Convert(longString);
        checkResult(result2, "超长字符串转换");
        
        // 5.3 只包含ASCII的字符串
        std::string asciiOnly = "Hello World 123!";
        auto result3 = helper.Convert(asciiOnly);
        checkResult(result3, "纯ASCII字符串转换");
        
    } catch (const std::exception& e) {
        printError("边界情况测试失败: " + std::string(e.what()));
    }
}

// 测试6: 往返转换（转换一致性）
void testRoundTripConversion() {
    printTestHeader("测试往返转换一致性");
    
    std::string original = "Hello 世界! 这是往返转换测试。包含各种字符：123 ABC abc ©®™";
    
    // UTF-8 -> GBK -> UTF-8
    auto toGbk = IconvHelper::Convert(original, "UTF-8", "GBK");
    if (toGbk.errorNumber == 0) {
        auto backToUtf8 = IconvHelper::Convert(toGbk.result, "GBK", "UTF-8");
        if (backToUtf8.errorNumber == 0) {
            if (original == backToUtf8.result) {
                printSuccess("UTF-8 -> GBK -> UTF-8 往返转换一致");
            } else {
                printError("UTF-8 -> GBK -> UTF-8 往返转换不一致");
                printInfo("原始长度: " + std::to_string(original.length()));
                printInfo("结果长度: " + std::to_string(backToUtf8.result.length()));
            }
        } else {
            printError("GBK -> UTF-8 转换失败");
        }
    } else {
        printError("UTF-8 -> GBK 转换失败");
    }
    
    // UTF-8 -> UTF-16LE -> UTF-8
    auto toUtf16 = IconvHelper::Convert(original, "UTF-8", "UTF-16LE");
    if (toUtf16.errorNumber == 0) {
        auto backToUtf8 = IconvHelper::Convert(toUtf16.result, "UTF-16LE", "UTF-8");
        if (backToUtf8.errorNumber == 0) {
            if (original == backToUtf8.result) {
                printSuccess("UTF-8 -> UTF-16LE -> UTF-8 往返转换一致");
            } else {
                printError("UTF-8 -> UTF-16LE -> UTF-8 往返转换不一致");
            }
        }
    }
}

int main() {
    std::system("chcp 65001 > nul");
    std::cout << "\033[36m";
    std::cout << "==========================================\n";
    std::cout << "         IconvHelper 全功能测试\n";
    std::cout << "==========================================\033[0m\n";
    
    try {
        testStaticConvert();
        testConstructorDestructor();
        testMoveSemantics();
        testInstanceConvert();
        testEdgeCases();
        testRoundTripConversion();
        
        std::cout << "\n\033[36m==========================================\n";
        std::cout << "              测试完成\n";
        std::cout << "==========================================\033[0m\n";
        
    } catch (const std::exception& e) {
        printError("测试过程中发生未捕获的异常: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
