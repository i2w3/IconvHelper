#include "IconvHelper.hpp"

#include <stdexcept>
#include <sstream>

IconvHelper::IconvHelper(char const *fromCode, char const *toCode) noexcept(false) 
: cd(iconv_open(toCode, fromCode)) {
    // 1. noexcept(false) 声明该构造函数可能会抛出异常
    // 2. 使用成员初始化列表而不是在构造函数体内赋值
    if (this->cd == (iconv_t)(-1)) {
        int error_code = errno;
        std::string error_message;
        switch (error_code) {
        case EINVAL:
            error_message = "不支持的编码";
            break;
        case ENOMEM:
            error_message = "内存不足";
            break;
        default:
            error_message = "未知错误";
            break;
        }
        std::string runtime_error = std::string(strerror(error_code)) + ": " + error_message;
        throw std::runtime_error(runtime_error);
    }
}

IconvHelper::~IconvHelper() noexcept {
    if (this->cd != (iconv_t)(-1)) {
        iconv_close(this->cd);
    }
}


IconvHelper::IconvHelper(IconvHelper&& ih) noexcept
: cd(ih.cd) {
    // 转移后，源对象 ih 的 cd 设为无效值，防止在源对象被析构时关闭已转移的资源
    ih.cd = (iconv_t)(-1); 
    // 源对象 ih 依然会正常析构，而不影响目标对象
}

IconvHelper& IconvHelper::operator=(IconvHelper&& ih) noexcept {
    if (this != &ih) { // 防止自我赋值 ihx = ihx
        iconv_close(this->cd); // 先析构自身
        this->cd = ih.cd; // 再拷贝源对象 ih
        ih.cd = (iconv_t)(-1); // 最后将源对象的 cd 设为无效值
    }
    return *this;
}

// static 关键字只在声明时使用，在定义时不需要
IConvResult IconvHelper::Convert(std::string_view in, char const* fromCode, char const* toCode) noexcept {
    IConvResult ir;
    iconv_t cd = iconv_open(toCode, fromCode);

    if(cd == (iconv_t)(-1)){
        ir.errorNumber = errno;
        switch(ir.errorNumber) {
            case EINVAL:
                ir.errorMessage = "不支持的编码";
                break;
            case ENOMEM:
                ir.errorMessage = "内存不足";
                break;
            default:
                ir.errorMessage = "未知错误";
                break;
        }
        return ir;
    }

    char* inBufferPtr = const_cast<char*>(in.data()); // 指向输入缓存位置 （非常量）
    size_t inBytesLeft = in.size(); // 输入缓存大小

    std::stringstream ss;

    const size_t sizeOfOutBuffer = 10; // 输出缓存区大小
    char outBuffer[sizeOfOutBuffer]; // 输出缓存

    while(inBytesLeft > 0) { // 输入缓存中，还有剩余字符未被转换
        char* outBufferPtr = outBuffer;
        size_t outBufferLeft = sizeOfOutBuffer; 

        // iconv 会自动调整 outBufferLeft 剩余大小
        size_t result = iconv(cd, &inBufferPtr, &inBytesLeft, &outBufferPtr, &outBufferLeft);

        if(result == (size_t)(-1)){ // 转换停止
            auto n = errno;
            switch(n){
                case E2BIG: {// 输出缓存区不够
                    break;
                }
                case EILSEQ: {
                    ir.errorNumber = n;
                    ir.errorMessage = "输入字符串不符合指定编码规则";
                    break;
                }
                case EINVAL: {
                    ir.errorNumber = n;
                    ir.errorMessage = "输入字符串不完整";
                    break;
                }
                default: {
                    ir.errorNumber = n;
                    ir.errorMessage = "转换过程发生未知错误";
                    break;
                }
            }
        }
        if (ir.errorNumber != 0) {
            break;
        }
        ss.write(outBuffer, sizeOfOutBuffer - outBufferLeft); // 将本轮的输出结果，写入输出流
    }

    iconv_close(cd);

    if (ir.errorNumber == 0){
        // ir.result = ss.str();
        ir.result = std::move(ss.str());
    }
    return ir;
}

IConvResult IconvHelper::Convert(std::string_view in) noexcept {
    IConvResult ir;
    char* inBufferPtr = const_cast<char*>(in.data()); // 指向输入缓存位置 （非常量）
    size_t inBytesLeft = in.size(); // 输入缓存大小

    std::stringstream ss;

    const size_t sizeOfOutBuffer = 10; // 输出缓存区大小
    char outBuffer[sizeOfOutBuffer]; // 输出缓存

    while(inBytesLeft > 0) { // 输入缓存中，还有剩余字符未被转换
        char* outBufferPtr = outBuffer;
        size_t outBufferLeft = sizeOfOutBuffer; 

        // iconv 会自动调整 outBufferLeft 剩余大小
        size_t result = iconv(cd, &inBufferPtr, &inBytesLeft, &outBufferPtr, &outBufferLeft);

        if(result == (size_t)(-1)){ // 转换停止
            auto n = errno;
            switch(n){
                case E2BIG: {// 输出缓存区不够
                    break;
                }
                case EILSEQ: {
                    ir.errorNumber = n;
                    ir.errorMessage = "输入字符串不符合指定编码规则";
                    break;
                }
                case EINVAL: {
                    ir.errorNumber = n;
                    ir.errorMessage = "输入字符串不完整";
                    break;
                }
                default: {
                    ir.errorNumber = n;
                    ir.errorMessage = "转换过程发生未知错误";
                    break;
                }
            }
        }
        if (ir.errorNumber != 0) {
            break;
        }
        ss.write(outBuffer, sizeOfOutBuffer - outBufferLeft); // 将本轮的输出结果，写入输出流
    }

    if (ir.errorNumber == 0){
        // ir.result = ss.str();
        ir.result = std::move(ss.str());
    }
    return ir;
}

std::string IconvHelper::getVersion() noexcept {
    return std::to_string(ICONVHELPER_VERSION_MAJOR) + "." +
           std::to_string(ICONVHELPER_VERSION_MINOR) + "." +
           std::to_string(ICONVHELPER_VERSION_PATCH);
}
