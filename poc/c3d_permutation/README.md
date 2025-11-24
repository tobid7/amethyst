# Citro3D Shader Buf info Permutation calculation

```cpp
// Why do 
BufInfo_Add(buf, data, sizeof(data[0]), 3, 0x210);
BufInfo_Add(buf, data, sizeof(data[0]), 4, 0x3210);
// if we can do 
BufInfo_Add(buf, data, sizeof(data[0]), 3, permutation(3));
BufInfo_Add(buf, data, sizeof(data[0]), 3, permutation(4));
```
