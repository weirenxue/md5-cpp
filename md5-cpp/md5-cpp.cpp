#include <iostream>

uint32_t uBitLeftRot(uint32_t, uint32_t);

void endianTransform(uint32_t*);

uint32_t F(uint32_t, uint32_t, uint32_t);
uint32_t G(uint32_t, uint32_t, uint32_t);
uint32_t H(uint32_t, uint32_t, uint32_t);
uint32_t I(uint32_t, uint32_t, uint32_t);

uint32_t FF(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t GG(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t HH(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t II(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

uint32_t com(uint32_t(*)(uint32_t, uint32_t, uint32_t), uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

int main()
{
	const uint32_t S[64] = {
		7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, \
		5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, \
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, \
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21	};

	// T[i] = (uint32_t)floor(abs(sin(i + 1)) * pow(2, 32))
	const uint32_t T[64] = {
		0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE, 0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501, 0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE, 0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821, \
		0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA, 0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8, 0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED, 0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A, \
		0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C, 0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70, 0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05, 0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665, \
		0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039, 0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1, 0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1, 0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391  };

	char msg[] = "Hello World!";
	uint64_t msgLenByte = strlen(msg);
	uint64_t msgLenBit = msgLenByte * 8;
	// 512 bit per groups
	uint64_t groups = msgLenBit % 512 >= 448 | msgLenBit % 512 == 0 ? (ceil(msgLenBit / 512.0) + 1) : (ceil(msgLenBit / 512.0));
	// 64 byte per groups
	uint64_t bufferLenByte = groups * 64;
	char *buffer = (char*)malloc(bufferLenByte);
	memset(buffer, NULL, bufferLenByte);
	memcpy(buffer, msg, msgLenByte);
	// padding bit 1 after message
	*(buffer + msgLenByte) = 0x80;
	// 64-bit message bit length
	for (char i = 0; i < 8; i++) {
		*(buffer + bufferLenByte - 8 + i ) = (msgLenBit >> (8 * i)) & 0xff;
	}

	uint32_t A = 0x67452301, B = 0xEFCDAB89, C = 0x98BADCFE, D = 0x10325476;
	uint32_t AA, BB, CC, DD;
	uint32_t X[16];
	for (uint64_t i = 0; i < groups; i++) {
		AA = A;
		BB = B;
		CC = C;
		DD = D;
		memcpy(X, buffer + (i * 64), 64);
		for (char j = 0; j < 64; j++) {
			if (j < 16) {
				AA = FF(AA, BB, CC, DD, X[j % 16], S[j], T[j]);
			}
			else if (j < 32) {
				AA = GG(AA, BB, CC, DD, X[(j * 5 + 1) % 16], S[j], T[j]);
			}
			else if (j < 48) {
				AA = HH(AA, BB, CC, DD, X[(j * 3 + 5) % 16], S[j], T[j]);
			}
			else {
				AA = II(AA, BB, CC, DD, X[(j * 7) % 16], S[j], T[j]);
			}
			uint32_t temp = AA;
			AA = DD;
			DD = CC;
			CC = BB;
			BB = temp;
		}
		A += AA;
		B += BB;
		C += CC;
		D += DD;
	}

	endianTransform(&A);
	endianTransform(&B);
	endianTransform(&C);
	endianTransform(&D);
	printf("%08x%08x%08x%08x", A, B, C, D);
}

uint32_t uBitLeftRot(uint32_t x, uint32_t s) {
	return (x << s) | (x >> (32 - s));
}

void endianTransform(uint32_t *x) {
	uint32_t temp = *x;
	*x = \
		((temp >> 24) & 0xff) | \
		((temp >> 16) & 0xff) << 8 | \
		((temp >> 8) & 0xff) << 16 | \
		((temp >> 0) & 0xff) << 24;
}

uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
	return (x & y) | ((~x) & z);
}
uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
	return (x & z) | (y & (~z));
}
uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
	return x ^ y ^ z;
}
uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
	return y ^ (x | (~z));
}

uint32_t FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t xk, uint32_t s, uint32_t ti) {
	return com(&F, a, b, c, d, xk, s, ti);
}
uint32_t GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t xk, uint32_t s, uint32_t ti) {
	return com(&G, a, b, c, d, xk, s, ti);
}
uint32_t HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t xk, uint32_t s, uint32_t ti) {
	return com(&H, a, b, c, d, xk, s, ti);
}
uint32_t II(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t xk, uint32_t s, uint32_t ti) {
	return com(&I, a, b, c, d, xk, s, ti);
}

uint32_t com(uint32_t(*auxiliaryFun)(uint32_t, uint32_t, uint32_t), uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t xk, uint32_t s, uint32_t ti) {
	return b + (uBitLeftRot(a + auxiliaryFun(b, c, d) + xk + ti, s));
}