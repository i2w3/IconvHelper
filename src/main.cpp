#include "IconvHelper.hpp"

#include <iostream>


int main() {
    std::system("chcp 65001 > nul");
    IconvHelper iconv("GBK", "UTF-8");
    extern const char* gbk_str;
    auto ir = iconv.Convert(gbk_str);
    if (ir.errorNumber != 0){
        // 转换失败
        std::cout << ir.errorNumber << " : " << ir.errorMessage << "\n";
    }
    else{
        // 转换成功
        std::cout << ir.result << std::endl;
    }
    return 0;
}
