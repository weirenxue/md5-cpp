# This project implements md5 in cpp.
### Enveironment: Visual Studio Pro 2017, Windows10 x64.  

## Algorithm
1. Append padding bits  
    Fill a bit 1 after the message, and then fill bit 0 to make the length equal to 448 (mod 512).
2. Append length  
    The original message length (mod 2^64) fills the last 64 bits in little-endian. The final message length (in bits) is a multiple of 512. 
    ```cpp
    Let the message be grouped by 32 bits and named M array(from 0 to length/32-1).
    ```
3. Initialize variable
    ```cpp
    32-bit register A is 0x67452301   =>  in memory &A  01 23 45 67
    32-bit register B is 0xEFCDAB89   =>  in memory &B  89 ab cd ef
    32-bit register C is 0x98BADCFE   =>  in memory &C  fe dc ba 98
    32-bit register D is 0x10325476   =>  in memory &D  76 54 32 10
    ```
    ```cpp
    32-bit array T with 64 element
    T[i] = floor( | sin(i + 1) | * 2^32 ), i = 0 to 63 in radian
    ``` 
    ```cpp
    bit shift array S with 64 element
    S[64] = {
		7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, \
		5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, \
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, \
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21	};
    ```
4. Data process data   
    1. The data is grouped every 512 bits. The number of groups is N.
    2. define auxiliary function
        ```
        F(X, Y, Z) = ((X & Y) | ((~X) & Z))
        G(X, Y, Z) = ((X & Z) | (Y & (~Z)))
        H(X, Y, Z) = (X ^ Y ^ Z)
        I(X, Y, Z) = (Y ^ (X | (~Z)))
        ```
    3. pesudo code
        ```py
        for i = 0 to (N - 1)
            (AA, BB, CC, DD) = (A, B, C, D)
            X[0..15] = M [(0..15) * i]
            for j = 0 to 64 
                if j < 16
                    AA = FF(AA, BB, CC, DD, X[j % 16], S[j], T[j]);
                else if j < 32
                    AA = GG(AA, BB, CC, DD, X[(j * 5 + 1) % 16], S[j], T[j]);
                else if j < 48
                    AA = HH(AA, BB, CC, DD, X[(j * 3 + 5) % 16], S[j], T[j]);
                else 
                    AA = II(AA, BB, CC, DD, X[(j * 7) % 16], S[j], T[j]);
                (AA, BB, CC, DD) = (DD, AA, BB, CC)
            (A, B, C, D) += (AA, BB, CC, DD)
        ```
    4. output
        ```py
        print ABCD in little endian format respectively
        ```
## Clear example 
Message is "Hello World!". in hex is 
```
48 65 6c 6c 6f 20 57 6f 72 6c 64 21
```
The original length is 96 bits (12 bytes * 8 bits/byte).  
1. Append padding bits  
    ```
                                                    ↓ a bit 1 after the message
    0x012298B0  48 65 6c 6c 6f 20 57 6f 72 6c 64 21 80 00 00 00  Hello World!€...
    0x012298C0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    0x012298D0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    0x012298E0  00 00 00 00 00 00 00 00                          ........
                                      ↑ here is 448 bits
    ```
2. Append length  
    96 = 0x60
    ```
    0x012298B0  48 65 6c 6c 6f 20 57 6f 72 6c 64 21 80 00 00 00  Hello World!€...
    0x012298C0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    0x012298D0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    0x012298E0  00 00 00 00 00 00 00 00 60 00 00 00 00 00 00 00  ........`.......
                                        ↑ is the lower byte of the original length
    ```
3. Initialize variable (is the same as algorithm part)
4. Data process data 
    1. The data is grouped every 512 bits. The number of groups is N. (is the same as algorithm part)
    2. define auxiliary function (is the same as algorithm part)
    3. pesudo code (is the same as algorithm part)
    4. output
        ```py
        A is 0x876207ed     =>  in memory &A  ed 07 62 87   (little endian)
		B is 0x36862e53     =>  in memory &A  53 2e 86 36   (little endian)
		C is 0x921e845e     =>  in memory &A  5e 84 1e 92   (little endian)
		D is 0x8c0dc5bf     =>  in memory &A  bf c5 0d 8c   (little endian)
        md5("Hello World!") is ed076287532e86365e841e92bfc50d8c
        ```
