#include <stdint.h>
#include <stdio.h>
#include <string.h>

//: is_ascii
int32_t is_ascii(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if ((unsigned char)str[i] > 127) {
            return 0;  // is not ascci
        }
    }
    return 1;  // is ascci
}

//capitalize_ascii
int32_t capitalize_ascii(char str[]) {
    int32_t count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 32;  // -32 to capitalize 
            count++;
        }
    }
    return count;
}

//width_from_start_byte
int32_t width_from_start_byte(char start_byte) {
    unsigned char byte = (unsigned char)start_byte;
    if (byte <= 127) return 1;                // ascci
    else if ((byte & 0xE0) == 0xC0) return 2; // 2byte
    else if ((byte & 0xF0) == 0xE0) return 3; // 3byte
    else if ((byte & 0xF8) == 0xF0) return 4; // 4byte
    return -1; // invalid start byte
}

//utf8_strlen
int32_t utf8_strlen(char str[]) {
    int32_t length = 0;
    for (int i = 0; str[i] != '\0';) {
        int width = width_from_start_byte(str[i]);
        if (width == -1) return -1; // invalid uft8
        i += width;
        length++;
    }
    return length;
}

//codepoint_index_to_byte_index
int32_t codepoint_index_to_byte_index(char str[], int32_t cpi) {
    int32_t byte_index = 0;
    for (int32_t i = 0; i < cpi; i++) {
        int width = width_from_start_byte(str[byte_index]);
        if (width == -1) return -1; // invalid UTF-8
        byte_index += width;
    }
    return byte_index;
}

//utf8_substring
void utf8_substring(char str[], int32_t cpi_start, int32_t cpi_end, char result[]) {
    if (cpi_start > cpi_end || cpi_start < 0 || cpi_end < 0) return;

    int32_t byte_index_start = codepoint_index_to_byte_index(str, cpi_start);
    int32_t byte_index_end = codepoint_index_to_byte_index(str, cpi_end);

    if (byte_index_start == -1 || byte_index_end == -1) return;

    int32_t length = byte_index_end - byte_index_start;
    strncpy(result, str + byte_index_start, length);
    result[length] = '\0'; // null terminator
}

// Function: codepoint_at
int32_t codepoint_at(char str[], int32_t cpi) {
    int32_t byte_index = codepoint_index_to_byte_index(str, cpi);
    if (byte_index == -1) return -1;

    unsigned char* ptr = (unsigned char*)&str[byte_index];
    int width = width_from_start_byte(str[byte_index]);

    if (width == 1) return ptr[0];
    if (width == 2) return ((ptr[0] & 0x1F) << 6) | (ptr[1] & 0x3F);
    if (width == 3) return ((ptr[0] & 0x0F) << 12) | ((ptr[1] & 0x3F) << 6) | (ptr[2] & 0x3F);
    if (width == 4) return ((ptr[0] & 0x07) << 18) | ((ptr[1] & 0x3F) << 12) | ((ptr[2] & 0x3F) << 6) | (ptr[3] & 0x3F);

    return -1;
}

// Function: is_animal_emoji_at
char is_animal_emoji_at(char str[], int32_t cpi) {
    int32_t codepoint = codepoint_at(str, cpi);

    // Animal emoji ranges: U+1F400 to U+1F43F, and U+1F980 to U+1F99F
    if ((codepoint >= 0x1F400 && codepoint <= 0x1F43F) ||
        (codepoint >= 0x1F980 && codepoint <= 0x1F99F)) {
        return 1;
    }
    return 0;
}

// UTF-8 Analyzer function
void utf8_analyzer(char str[]) {
    printf("Enter a UTF-8 encoded string: %s\n", str);

    // ASCII Check
    int ascii_check = is_ascii(str);
    printf("Valid ASCII: %s\n", ascii_check ? "true" : "false");

    // Uppercase ASCII
    char upper_str[256];
    strcpy(upper_str, str);
    int uppercase_count = capitalize_ascii(upper_str);
    printf("Uppercased ASCII: \"%s\"\n", upper_str);

    // Length in bytes
    int length_in_bytes = strlen(str);
    printf("Length in bytes: %d\n", length_in_bytes);

    // Number of codepoints
    int codepoints = utf8_strlen(str);
    printf("Number of code points: %d\n", codepoints);

    // Bytes per code point
    printf("Bytes per code point: ");
    for (int i = 0; i < codepoints; i++) {
        int byte_index = codepoint_index_to_byte_index(str, i);
        if (byte_index != -1) {
            printf("%d ", width_from_start_byte(str[byte_index]));
        }
    }
    printf("\n");

    // Substring of the first 6 codepoints
    char substring[64];
    utf8_substring(str, 0, 6, substring);
    printf("Substring of the first 6 code points: \"%s\"\n", substring);

    // Codepoints as decimal numbers
    printf("Code points as decimal numbers: ");
    for (int i = 0; i < codepoints; i++) {
        int32_t cp = codepoint_at(str, i);
        if (cp != -1) {
            printf("%d ", cp);
        }
    }
    printf("\n");

    // List of animal emojis
    printf("Animal emojis: ");
    for (int i = 0; i < codepoints; i++) {
        if (is_animal_emoji_at(str, i)) {
            int byte_index = codepoint_index_to_byte_index(str, i);
            int width = width_from_start_byte(str[byte_index]);
            for (int j = 0; j < width; j++) {
                printf("%c", str[byte_index + j]);
            }
            printf(" ");
        }
    }
    printf("\n");

// Next Character of Codepoint at Index 3
    int32_t next_cp = codepoint_at(str, 3) + 1;
    printf("Next Character of Codepoint at Index 3: ");
    if (next_cp >= 0x1F400 && next_cp <= 0x1F99F) { // Check if it's a valid animal emoji range
        char next_char[5] = {0};
        int width = width_from_start_byte(str[3]);
        if (width == 4) { // Encoding for 4-byte UTF-8
            next_char[0] = 0xF0 | ((next_cp >> 18) & 0x07);
            next_char[1] = 0x80 | ((next_cp >> 12) & 0x3F);
            next_char[2] = 0x80 | ((next_cp >> 6) & 0x3F);
            next_char[3] = 0x80 | (next_cp & 0x3F);
            printf("%s\n", next_char);
        }
    } else {
        printf("No next character in the range.\n");
    }
}

int main() {
    
    //test is_ascii
    printf("Is 🔥 ASCII? %d\n", is_ascii("🔥")); //0=not ascci
    printf("Is abcd ASCII? %d\n", is_ascii("abcd")); //1=isascci

    //test capitalize_ascii
    int32_t ret = 0;
    char str1[] = "abcd";
    ret = capitalize_ascii(str1);
    printf("Capitalized String: %s\nCharacters updated: %d\n", str1, ret); 

    //test width_from_start_byte
    char s[] = "Héy";
    printf("Width: %d bytes\n", width_from_start_byte(s[1])); 
    printf("Width: %d bytes\n", width_from_start_byte(s[2])); 

    //test utf8_strlen
    char str2[] = "Joséph";
    printf("Length of string %s is %d\n", str2, utf8_strlen(str2));

    //test codepoint_index_to_byte_index
    char str3[] = "Joséph";
    int32_t idx = 4;
    printf("Codepoint index %d is byte index %d\n", idx, codepoint_index_to_byte_index(str3, idx));

    //test utf8_substring
    char result[17];
    utf8_substring("🦀🦮🦮🦀🦀🦮🦮", 3, 7, result);
    printf("Substring: %s\n", result); //output the pointcode at 3to7

    // Test cases and example run for UTF-8 Analyzer
    char str[] = "My 🐩’s name is Erdős.";
    utf8_analyzer(str);

    return 0;
}
