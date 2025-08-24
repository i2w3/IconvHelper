# IconvHelper
一个自定义的 [libiconv](https://www.gnu.org/software/libiconv/) 封装类 IconvHelper

## 自编译
```powershell
vcpkg new --name=iconvhelper --version=1.0.0
vcpkg add port libiconv
cmake --preset release
cmake --build build/release
```

- 可选：编译测试程序
```
cmake --build build/release --target test
```