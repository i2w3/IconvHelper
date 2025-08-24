#ifndef _ICONVHELPER
#define _ICONVHELPER

#include <iconv.h>

#include <string>
#include <string_view>
#include "version.h"

// 转换结果
struct IConvResult{
    std::string result; // 转换成功得到的，使用新编码的字符串

    int errorNumber = 0; // 对应 errno, 出错号
    std::string errorMessage; // 出错信息
};

class IconvHelper final {// 注：实现为 final 类，阻止进一步继承或重写
public:
    // 默认带参数构造（失败时可抛出异常）
    IconvHelper(char const* fromCode, char const* toCode) noexcept(false);
    IconvHelper(IconvHelper const& ) = delete; // 不允许复制 IconvHelper ih2 = ih1;
    IconvHelper& operator = (IconvHelper const&) = delete;  // 不允许赋值 ih2 = ih1;

    IconvHelper(IconvHelper&& ih) noexcept; // 支持转移 IconvHelper ih2 = std::move(ih1);
    IconvHelper& operator = (IconvHelper&& ih) noexcept;  // 支持转移赋值 ih2 = std::move(ih1);

    ~IconvHelper() noexcept; // 析构

    // 获取版本号
    static std::string getVersion() noexcept;

    // 静态转换，方便一次性转换 (不允许抛出异常)
    static IConvResult Convert(std::string_view in, char const* fromCode, char const* toCode) noexcept;

    // 非静态的转换方法，方便多次复用 (不允许抛出异常)
    IConvResult Convert(std::string_view in) noexcept;  

private:
    iconv_t cd;
};

#endif // _ICONVHELPER