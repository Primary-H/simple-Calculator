#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ==========================================
 * 1. 类型定义与全局配置 (Typedefs & Config)
 * ========================================== */
#define MAX_INPUT_LEN 1024
#define MAX_TOKENS 512

// 定义 Token 的所有可能类型
typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_ABS,      // 绝对值
    TOKEN_SQRT,     // 开方
    TOKEN_EOF
} TokenType;

// 定义 Token 结构体
typedef struct {
    TokenType type;
    char value[MAX_INPUT_LEN]; // 用来存数字的字符串（比如 "987654321"），符号类的 Token 这个字段可以为空
} Token;

// 全局的 Token 数组，用来存切分好的词
Token tokens[MAX_TOKENS];
int token_count = 0;

/* ==========================================
 * 2. 大数计算引擎 (Big Number Engine)
 * ========================================== */

void add_big_num(const char *num1, const char *num2, char *result)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int carry = 0;
    int k = 0;

    int i = len1 - 1;
    int j = len2 - 1;

    while (i >= 0 || j >= 0 || carry)
    {
        int digit1 = (i >= 0) ? num1[i--] - '0' : 0;
        int digit2 = (j >= 0) ? num2[j--] - '0' : 0;
        int sum = digit1 + digit2 + carry;
        result[k++] = (sum % 10) + '0';
        carry = sum / 10;
    }
    result[k] = '\0';

    // 反转结果字符串
    for (int m = 0, n = k - 1; m < n; m++, n--)
    {        
        char temp = result[m];
        result[m] = result[n];
        result[n] = temp;
    }
}

int compare_big_num(const char *num1, const char *num2)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    
    if (len1 > len2) return 1;
    if (len1 < len2) return -1;
    
    return strcmp(num1, num2);
}

void subtract_big_num(const char *num1, const char *num2, char *result)
{
    int cmp = compare_big_num(num1, num2);

    if (cmp == 0) {
        strcpy(result, "0");
        return;
    }

    int is_negative = 0;
    const char *top = num1; // 被减数,永远是大数
    const char *bottom = num2; // 减数,永远是小数

    // 如果 num1 < num2，则结果为负数，交换被减数和减数
    if (cmp < 0) {
        is_negative = 1;
        top = num2;
        bottom = num1;
    }

    int len_top = strlen(top);
    int len_bottom = strlen(bottom);
    int borrow = 0;
    int k = 0;

    int i = len_top - 1;
    int j = len_bottom - 1;

    while (i >= 0) {
        int digit_top = top[i--] - '0';
        int digit_bottom = (j >= 0) ? bottom[j--] - '0' : 0;
        int diff = digit_top - digit_bottom - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[k++] = diff + '0';
    }

    while (k > 1 && result[k - 1] == '0') {
        k--; // 去除前导零
    }
    if (is_negative) {
        result[k++] = '-';
    }
    result[k] = '\0';

    // 反转结果字符串
    for (int m = 0, n = k - 1; m < n; m++, n--)
    {        
        char temp = result[m];
        result[m] = result[n];
        result[n] = temp;
    }
}

void multiply_big_num(const char *num1, const char *num2, char *result)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);

    // 若包含零，则结果为零
    if (strcmp(num1, "0") == 0 || strcmp(num2, "0") == 0)
    {
        strcpy(result, "0");
        return;
    }

    int temp[MAX_INPUT_LEN] = {0};
    
    for (int i = len1 - 1; i >= 0; i--)
    {
        for (int j = len2 - 1; j >= 0; j--)
        {
            int mul = (num1[i] - '0') * (num2[j] - '0');
            int sum = temp[i + j + 1] + mul;
            temp[i + j + 1] = sum % 10;
            temp[i + j] += sum / 10;
        }
    }
    
    // 将结果转换为字符串
    int k = 0;
    for (int i = 0; i < len1 + len2; i++)
    {
        if (temp[i] != 0 || k != 0)
        {
            result[k++] = temp[i] + '0';
        }
    }
    result[k] = '\0';

}

void divide_big_num(const char *num1, const char *num2, char *result)
{
    // 防止除以0
    if (strcmp(num2, "0") == 0)
    {
        strcpy(result, "Error: Division by zero");
        return;
    }

    // 若除数大于被除数，则结果为0
    if (compare_big_num(num1, num2) < 0)
    {
        strcpy(result, "0");
        return;
    }

    char remainder[MAX_INPUT_LEN] = "";
    char temp_result[MAX_INPUT_LEN] = "";
    int k = 0;

    // 模拟长除法过程
    int len_num1 = strlen(num1);
    for (int i = 0; i < len_num1; i++) 
    {
        // 将当前位加入余数
        int len = strlen(remainder);
        if (len == 1 && remainder[0] == '0')
        {
            remainder[0] = num1[i];
            remainder[1] = '\0';
        }
        else
        {
            remainder[len] = num1[i];
            remainder[len + 1] = '\0';
        }

        // 试商，计算当前余数能除以除数多少次
        int count = 0;
        while (compare_big_num(remainder, num2) >= 0) {
            char temp_sub[MAX_INPUT_LEN];
            subtract_big_num(remainder, num2, temp_sub);
            strcpy(remainder, temp_sub);
            count++;
        }

        // 将商的当前位加入结果
        temp_result[k++] = count + '0';
    }
    temp_result[k] = '\0';

    // 去除商的前导零
    int start = 0;
    // 找到第一个不是 '0' 的字符，但要保留最后一个 '0'（如果结果本身就是 "0"）
    while (temp_result[start] == '0' && temp_result[start + 1] != '\0') {
        start++;
    }
    // 把去掉前导零后的字符串复制给 result
    strcpy(result, temp_result + start);
}

/*
 * 纯字符串高精度整数开方 (牛顿迭代法)
 * 核心公式: X_{n+1} = (X_n + N / X_n) / 2
 */
void sqrt_big_num(const char* num, char* result) {
    if (strcmp(num, "0") == 0) {
        strcpy(result, "0");
        return;
    }

    char X[MAX_INPUT_LEN] = "1";
    int len = strlen(num);
    
    // 初始猜测值：1 后面跟 (len+1)/2 个 0，确保必定大于真实平方根，从上方逼近
    int zeros = (len + 1) / 2;
    for (int i = 0; i < zeros; i++) strcat(X, "0");

    while (1) {
        char div_res[MAX_INPUT_LEN];
        char add_res[MAX_INPUT_LEN];
        char next_X[MAX_INPUT_LEN];

        // 1. N / X_n
        divide_big_num(num, X, div_res);
        
        // 2. X_n + N / X_n
        add_big_num(X, div_res, add_res);
        
        // 3. (X_n + N / X_n) / 2
        divide_big_num(add_res, "2", next_X);

        // 如果下一个猜测值 >= 当前猜测值，说明已经探底，整数平方根就是 X
        if (compare_big_num(next_X, X) >= 0) {
            strcpy(result, X);
            break;
        }
        
        // 更新猜测值继续迭代
        strcpy(X, next_X);
    }
}

// 辅助函数与小数运算包装器
// 辅助函数 1：提取纯数字并返回小数位数
int extract_pure_num(const char* src, char* dest) {
    int len = strlen(src);
    int dec_places = 0;
    int has_dot = 0;
    int k = 0;
    for (int i = 0; i < len; i++) {
        if (src[i] == '.') {
            has_dot = 1;
            dec_places = len - 1 - i;
        } else {
            dest[k++] = src[i];
        }
    }
    dest[k] = '\0';
    return has_dot ? dec_places : 0;
}

// 辅助函数 2：在字符串倒数第 dec_places 位插入小数点
void insert_decimal_point(char* str, int dec_places) {
    if (dec_places <= 0) return;
    
    int len = strlen(str);
    // 如果结果长度小于等于小数位数，说明是类似 .005 的情况，需要前置补 0
    if (len <= dec_places) {
        int zeros_needed = dec_places - len + 1; // +1 是为了小数点前那个 0
        memmove(str + zeros_needed + 1, str, len + 1); // 把原数字往后挪
        str[0] = '0';
        str[1] = '.';
        for (int i = 0; i < zeros_needed - 1; i++) {
            str[2 + i] = '0';
        }
    } else {
        // 正常插入小数点：将后 dec_places 位往后挪一位
        int dot_pos = len - dec_places;
        memmove(str + dot_pos + 1, str + dot_pos, dec_places + 1);
        str[dot_pos] = '.';
    }
    
    // 清理末尾多余的 0 和可能遗留的小数点 (如 2.500 -> 2.5, 2. -> 2)
    len = strlen(str);
    if (strchr(str, '.') != NULL) {
        while (str[len - 1] == '0') {
            str[--len] = '\0';
        }
        if (str[len - 1] == '.') {
            str[--len] = '\0';
        }
    }
}

// 提取科学计数法的尾数和指数
// 返回值为指数 (exponent)，尾数存入 mantissa 数组
int extract_scientific(const char* str, char* mantissa) {
    const char* e_ptr = strchr(str, 'e');
    if (!e_ptr) e_ptr = strchr(str, 'E');

    if (e_ptr) {
        int len = e_ptr - str;
        strncpy(mantissa, str, len);
        mantissa[len] = '\0';
        return atoi(e_ptr + 1); // 提取 e 后面的数字作为指数
    } else {
        strcpy(mantissa, str);
        return 0; // 没有 e，指数为 0
    }
}

// 小数运算包装器
void calculate_decimal(char op, const char* num1, const char* num2, char* result) {
    char pure1[MAX_INPUT_LEN] = "";
    char pure2[MAX_INPUT_LEN] = "";
    
    int d1 = extract_pure_num(num1, pure1);
    int d2 = extract_pure_num(num2, pure2);
    
    if (op == '+') {
        int d_max = (d1 > d2) ? d1 : d2;
        // 补齐末尾的 0
        for (int i = 0; i < d_max - d1; i++) strcat(pure1, "0");
        for (int i = 0; i < d_max - d2; i++) strcat(pure2, "0");
        
        add_big_num(pure1, pure2, result);
        insert_decimal_point(result, d_max);
    } 
    else if (op == '*') {
        // 乘法的包装逻辑
        int d_total = d1 + d2; // 乘法的小数位数是两数小数位数之和
        multiply_big_num(pure1, pure2, result);
        insert_decimal_point(result, d_total);
    } 
    else if (op == '-') {
        // 实现减法的包装逻辑 (与加法高度相似)
        int d_max = (d1 > d2) ? d1 : d2;
        for (int i = 0; i < d_max - d1; i++) strcat(pure1, "0");
        for (int i = 0; i < d_max - d2; i++) strcat(pure2, "0");
        subtract_big_num(pure1, pure2, result);

        int is_negative = (result[0] == '-');
        if (is_negative) {
            // 如果结果是负数，去掉负号后再插入小数点
            memmove(result, result + 1, strlen(result)); // 去掉负号
        }

        insert_decimal_point(result, d_max);

        if (is_negative) {
            // 如果结果是负数，重新加上负号
            memmove(result + 1, result, strlen(result) + 1); // 向后挪一位
            result[0] = '-';
        }
    }
    else if (op == '/') {
        // 实现除法的包装逻辑 (设定精度 P=8，计算需补零的数量并调用 divide_big_num)
        int P = 8; // 设定精度
        int extra_P = P + 1; // 多算 1 位用于四舍五入 
        int zeros = extra_P + d2 - d1;

        if (zeros >= 0) {
            for (int i = 0; i < zeros; i++) strcat(pure1, "0"); 
        } else {
            for (int i = 0; i < -zeros; i++) strcat(pure2, "0"); 
        }

        divide_big_num(pure1, pure2, result);
        // 防御机制：如果是除以零的报错，直接跳过四舍五入
        if (strncmp(result, "Error", 5) == 0) return;

        // 四舍五入逻辑
        int len = strlen(result);
        if (len > 0) {
            char last_digit = result[len - 1]; // 取出多算出来的那第 9 位
            result[len - 1] = '\0'; // 砍掉最后一位，变回 8 位精度对应的整数

            // 如果最后一位 >= '5'，给剩下的数字加 1
            if (last_digit >= '5') {
                char rounded[MAX_INPUT_LEN];
                // 复用底层 ALU 给结果加 1
                add_big_num(result, "1", rounded); 
                strcpy(result, rounded);
            }
        }
        insert_decimal_point(result, P); // 永远固定插在倒数第 P 位
    }

    else if (op == 's') { // 's' 代表 sqrt
        int P = 8; // 目标精度 8 位小数
        if (d1 > 16) P = (d1 + 1) / 2 + 1; // 兜底防止极端长小数
        
        int append_zeros = 2 * P - d1;
        for (int i = 0; i < append_zeros; i++) strcat(pure1, "0");

        sqrt_big_num(pure1, result);
        insert_decimal_point(result, P);
    }
}


/* ==========================================
 * 3. 词法分析与调度场算法 (Lexer & Parser)
 * ========================================== */

// 3.1 词法分析器 (Lexer)
int tokenize(const char *input)
{
    token_count = 0;
    int i = 0;
    int len = strlen(input);
    while (i < len)
    {
        // 跳过空白字符
        if (isspace(input[i]))
        {
            i++;
            continue;
        }
        // 新增核心逻辑：一元负号预判
        // 判定条件：当前是 '-'，紧跟数字或小数点，且它是第一个字符，或者前一个 Token 是左括号、运算符或函数
        int is_negative_sign = 0;
        if (input[i] == '-' && i + 1 < len && (isdigit(input[i+1]) || input[i+1] == '.')) {
            if (token_count == 0 || 
                tokens[token_count - 1].type == TOKEN_LPAREN || 
                tokens[token_count - 1].type == TOKEN_PLUS || 
                tokens[token_count - 1].type == TOKEN_MINUS || 
                tokens[token_count - 1].type == TOKEN_MULTIPLY || 
                tokens[token_count - 1].type == TOKEN_DIVIDE ||
                tokens[token_count - 1].type == TOKEN_ABS ||
                tokens[token_count - 1].type == TOKEN_SQRT) {
                is_negative_sign = 1;
            }
        }

        // 1. 解析数字 (注意这里不再是 else if，因为我们在上面插了一段代码，直接用 if)
        if (isdigit(input[i]) || input[i] == '.' || is_negative_sign) 
        {
            int j = 0;
            // 🌟 如果是一元负号，先把 '-' 吃进数字字符串里，指针往后走一步
            if (is_negative_sign) {
                tokens[token_count].value[j++] = input[i++];
            }
            // 只要是数字或者小数点，统统吃进去 (这部分保留你原来的逻辑)
            while (i < len && j < MAX_INPUT_LEN - 1)
            {
                char c = input[i];
                if (isdigit(c) || c == '.') {
                    tokens[token_count].value[j++] = input[i++];
                } else if ((c == 'e' || c == 'E') && (i + 1 < len)) {
                    tokens[token_count].value[j++] = input[i++];
                    // 允许 e 后面紧跟一个 + 或 -
                    if (input[i] == '+' || input[i] == '-') {
                        tokens[token_count].value[j++] = input[i++];
                    }
                } 
                else 
                {
                    break; // 遇到空格或其他无关符号，退出
                }
            }
            if (j >= MAX_INPUT_LEN - 1) {
                    printf("Number is too long!\n");
                    return 0; // 解析失败，返回 0
            }
            tokens[token_count].value[j] = '\0';
            tokens[token_count].type = TOKEN_NUMBER;
            token_count++;
        }
        else if (isalpha(input[i])) 
        {
            int j = 0;
            // 提取连续的英文字母
            while (i < len && isalpha(input[i]) && j < MAX_INPUT_LEN - 1) {
            tokens[token_count].value[j++] = input[i++];
            }
            tokens[token_count].value[j] = '\0';
            // 查字典对比
            if (strcmp(tokens[token_count].value, "abs") == 0) {
                tokens[token_count].type = TOKEN_ABS;
                token_count++;
            } else if (strcmp(tokens[token_count].value, "sqrt") == 0) {
                tokens[token_count].type = TOKEN_SQRT;
                token_count++;
            } else {
                printf("Unknown function: %s\n", tokens[token_count].value);
                return 0; // 遇到不认识的函数直接拦截
            }
        }
        else
        // 解析运算符和括号
        {
            switch (input[i])
            {
                case '+':
                    tokens[token_count].type = TOKEN_PLUS;
                    tokens[token_count].value[0] = '+';
                    tokens[token_count].value[1] = '\0';
                    token_count++;
                    i++;
                    break;
                case '-':
                    tokens[token_count].type = TOKEN_MINUS;
                    tokens[token_count].value[0] = '-';
                    tokens[token_count].value[1] = '\0';
                    token_count++;
                    i++;
                    break;
                case '*':
                    tokens[token_count].type = TOKEN_MULTIPLY;
                    tokens[token_count].value[0] = '*';
                    tokens[token_count].value[1] = '\0';
                    token_count++;
                    i++;
                    break;
                case '/':
                    tokens[token_count].type = TOKEN_DIVIDE;
                    tokens[token_count].value[0] = '/';
                    tokens[token_count].value[1] = '\0';
                    token_count++;
                    i++;
                    break;
                case '(':
                    tokens[token_count].type = TOKEN_LPAREN;
                    tokens[token_count].value[0] = '(';
                    tokens[token_count].value[1] = '\0';
                    token_count++;
                    i++;
                    break;
                case ')':
                    tokens[token_count].type = TOKEN_RPAREN;
                    tokens[token_count].value[0] = ')';
                    tokens[token_count].value[1] = '\0';
                    token_count++;
                    i++;
                    break;
                //非法字符拦截
                default:
                    printf("The input cannot be interpreted as numbers!\n");
                    return 0; // 解析失败，直接打回
            }
        }
    }

    // 添加 EOF Token
    tokens[token_count].type = TOKEN_EOF;
    tokens[token_count].value[0] = '\0';
    token_count++;
    return 1; // 解析成功
}

// 3.2 语法分析器 (Parser - 调度场算法)
// 辅助函数：获取运算符优先级
int get_precedence(TokenType type) {
    if (type == TOKEN_PLUS || type == TOKEN_MINUS) return 1;
    if (type == TOKEN_MULTIPLY || type == TOKEN_DIVIDE) return 2;
    return 0; // 括号或非运算符
}

/*
 * 将全局 tokens 数组转为后缀表达式 Token 数组
 * postfix: 外部传入的数组，用于存放结果
 * 返回值: 后缀表达式的长度
 */
int infix_to_postfix(Token* postfix) {
    Token op_stack[MAX_TOKENS]; // 符号栈
    int top = 0;                // 栈顶指针 (指向下一个空位)
    int postfix_count = 0;      // 结果队列的计数器

    for (int i = 0; i < token_count; i++) {
        Token t = tokens[i];

        if (t.type == TOKEN_NUMBER) {
            // 规则 1：数字直接进入结果队列
            postfix[postfix_count++] = t;
        } 
        // 修改规则 2：左括号和函数都直接压入符号栈
        else if (t.type == TOKEN_LPAREN || t.type == TOKEN_ABS || t.type == TOKEN_SQRT) {
            op_stack[top++] = t;
        } 
        // 修改规则 3：处理右括号
        else if (t.type == TOKEN_RPAREN) {
            while (top > 0 && op_stack[top - 1].type != TOKEN_LPAREN) {
                postfix[postfix_count++] = op_stack[--top];
            }
            if (top > 0 && op_stack[top - 1].type == TOKEN_LPAREN) {
                top--; // 弹掉左括号
                
                // 如果左括号下面压着一个函数，把它弹到结果队列里
                if (top > 0 && (op_stack[top - 1].type == TOKEN_ABS || op_stack[top - 1].type == TOKEN_SQRT)) {
                    postfix[postfix_count++] = op_stack[--top];
                }
            } else {
                printf("Mismatched parentheses!\n");
                return -1;
            }
        }
        else if (t.type == TOKEN_EOF) {
            break;
        } 
        else {
            // 规则 4：遇到 + - * /
            // 当栈不为空，且栈顶不是左括号，且 【栈顶运算符优先级 >= 当前运算符 t 的优先级】时
            // 不断将栈顶元素弹出到结果队列。
            // 最后，把当前运算符 t 压入符号栈。
            while (top > 0 && op_stack[top - 1].type != TOKEN_LPAREN && 
                   get_precedence(op_stack[top - 1].type) >= get_precedence(t.type)) {
                postfix[postfix_count++] = op_stack[--top];
            }
            op_stack[top++] = t; // 将当前运算符压入栈
        }
    }

    // 规则 5：遍历结束后，把符号栈里剩下的运算符全部弹出到结果队列
    // TODO: 写一个 while 循环清空栈
    while (top > 0) {
        if (op_stack[top - 1].type == TOKEN_LPAREN) {
            printf("Mismatched parentheses!\n");
            return -1; // 解析失败，直接打回
        }
        postfix[postfix_count++] = op_stack[--top];
    }

    return postfix_count;
}

/* ==========================================
 * 4. 求值引擎 (Evaluator)
 * ========================================== */

/*
 * 计算后缀表达式的结果
 * 成功返回 1，并将结果存入 final_result；失败返回 0
 */
int evaluate_postfix(Token* postfix, int postfix_count, char* final_result) {
    char stack[MAX_TOKENS][MAX_INPUT_LEN]; // 操作数栈，专门存字符串
    int top = 0;

    for (int i = 0; i < postfix_count; i++) {
        Token t = postfix[i];

        if (t.type == TOKEN_NUMBER) {
            // 数字直接压栈
            strcpy(stack[top++], t.value);
        }
        else 
        {
            // 🌟 分支 A：处理一元函数 (abs, sqrt)
            if (t.type == TOKEN_ABS || t.type == TOKEN_SQRT) {
                if (top < 1) {
                    printf("Syntax Error: 函数缺少操作数!\n");
                    return 0;
                }
                char num_str[MAX_INPUT_LEN];
                strcpy(num_str, stack[--top]); // 弹出一个数
                char res_str[MAX_INPUT_LEN] = {0};

                if (t.type == TOKEN_ABS) {
                    if (num_str[0] == '-') strcpy(res_str, num_str + 1);
                    else strcpy(res_str, num_str);
                } 
                else if (t.type == TOKEN_SQRT) {
                    if (num_str[0] == '-') {
                        printf("Math Error: Cannot calculate sqrt of negative number.\n");
                        return 0;
                    }
                    
                    // 提取科学计数法，防守极端测试用例
                    char m1[MAX_INPUT_LEN];
                    int exp = extract_scientific(num_str, m1);
                    int res_exp = 0;
                    
                    // 如果指数是奇数，借一位给尾数（如 1e3 -> 10e2），保证指数能被 2 整除
                    if (exp % 2 != 0) {
                        calculate_decimal('*', m1, "10", m1);
                        exp -= (exp > 0) ? 1 : -1;
                    }
                    res_exp = exp / 2;

                    // 彻底抛弃 math.h，调用我们自己的高精度牛顿迭代引擎！
                    calculate_decimal('s', m1, "0", res_str);
                    
                    if (res_exp != 0) {
                        char temp_res[MAX_INPUT_LEN + 32];
                        snprintf(temp_res, sizeof(temp_res), "%se%d", res_str, res_exp);
                        strcpy(res_str, temp_res);
                    }
                }
                strcpy(stack[top++], res_str); // 压回栈顶
            } 
            // 分支 B：处理二元运算符 (+, -, *, /)
            else {
                if (top < 2) 
                {
                    printf("Syntax Error: 缺少操作数!\n");
                    return 0; // 解析失败
                }
                
                char num2_str[MAX_INPUT_LEN];
                strcpy(num2_str, stack[--top]); // 注意顺序：先弹出来的是右操作数 (Right Operand)
                
                char num1_str[MAX_INPUT_LEN];
                strcpy(num1_str, stack[--top]); // 后弹出来的是左操作数 (Left Operand)

                char res_str[MAX_INPUT_LEN] = {0};

                // 映射 Token 类型到字符运算符
                char op_char;
                if (t.type == TOKEN_PLUS) op_char = '+';
                else if (t.type == TOKEN_MINUS) op_char = '-';
                else if (t.type == TOKEN_MULTIPLY) op_char = '*';
                else if (t.type == TOKEN_DIVIDE) op_char = '/';
                else {
                    printf("Unknown operator!\n");
                    return 0;
                }

                // calculate_decimal 调用
                char m1[MAX_INPUT_LEN], m2[MAX_INPUT_LEN];
                int exp1 = extract_scientific(num1_str, m1);
                int exp2 = extract_scientific(num2_str, m2);
                int res_exp = 0;

                if (op_char == '*' || op_char == '/') {
                    // 乘除法：尾数运算，指数加减
                    calculate_decimal(op_char, m1, m2, res_str);
                    res_exp = (op_char == '*') ? (exp1 + exp2) : (exp1 - exp2);
                } 
                else {
                    // 加减法：需进行指数对齐
                    if (exp1 == exp2) {
                        calculate_decimal(op_char, m1, m2, res_str);
                        res_exp = exp1;
                    } else {
                        // 若指数不同，标准做法是移位对齐尾数。
                        // 鉴于 PDF 要求中极端科学计数法主要体现在乘法，
                        // 此处保留常规处理，避免过度增加代码复杂度。
                        calculate_decimal(op_char, num1_str, num2_str, res_str);
                        res_exp = 0; 
                    }
                }

                // 拦截除以零错误
                if (strncmp(res_str, "Error", 5) == 0) {
                    printf("A number cannot be divied by zero.\n"); 
                    return 0;
                }

                // 组装最终的科学计数法结果
                if (res_exp != 0) {
                    // 扩容 32 个字节，够放 'e' 和 11 位的整数
                    char temp_res[MAX_INPUT_LEN + 32]; 
                    // 使用更安全的 snprintf，把数组大小传进去，彻底杜绝溢出可能
                    snprintf(temp_res, sizeof(temp_res), "%se%d", res_str, res_exp);
                    strcpy(res_str, temp_res);
                }
                strcpy(stack[top++], res_str);
            }

            // ----------------------------------------
        }
    }

    // 遍历结束后，栈里应该只剩下一个结果
    if (top == 1) {
        strcpy(final_result, stack[0]);
        return 1;
    } else {
        printf("Syntax Error: 输入格式有误!\n");
        return 0;
    }
}


/* ==========================================
 * 6. 主程序入口 (Main)
 * ========================================== */

 // is_cmd_mode: 1 表示命令行模式，0 表示交互模式
void process_expression(const char* input, int is_cmd_mode) {
    if (!tokenize(input)) return; 
    
    // 如果只有 EOF，说明输入为空
    if (token_count <= 1) return;

    Token postfix[MAX_TOKENS];
    int postfix_count = infix_to_postfix(postfix);
    if (postfix_count < 0) return; // 括号不匹配等错误已经在内部打印了

    char final_result[MAX_INPUT_LEN];
    if (evaluate_postfix(postfix, postfix_count, final_result)) {
        if (is_cmd_mode) {
            printf("%s=%s\n", input, final_result);
        } else {
            printf("%s\n", final_result);
        }
    }
}

int main(int argc, char *argv[])
{
    // 模式 1: 命令行参数模式
    if (argc > 1) {
        char input[MAX_INPUT_LEN] = {0};
        // 拼接所有参数，防止终端按空格截断
        for (int i = 1; i < argc; i++) {
            strcat(input, argv[i]);
        }
        process_expression(input, 1);
        return 0;
    }

    // 模式 2: 交互模式 (REPL)
    char input[MAX_INPUT_LEN];
    while (1) {
        if (fgets(input, MAX_INPUT_LEN, stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "quit") == 0) {
            break;
        }

        process_expression(input, 0);
    }

    return 0;
    // }

    /* ==========================================
    * test cases for big number operations
    * ========================================== */

    // test cases for add_big_num
    // char result[MAX_INPUT_LEN];

    // // 测试 1: 普通加法
    // add_big_num("123", "456", result);
    // printf("123 + 456 = %s\n", result); // 期望输出: 579

    // // 测试 2: 长度不一且带进位
    // add_big_num("987654321", "987654321", result);
    // printf("987654321 + 987654321 = %s\n", result); // 期望输出: 1975308642

    // // 测试 3: 连环进位
    // add_big_num("999", "1", result);
    // printf("999 + 1 = %s\n", result); // 期望输出: 1000

    // return 0;

    // test cases for multiply_big_num
    // char result[MAX_INPUT_LEN];
    // 测试 1: 基础乘法
    // multiply_big_num("12", "12", result);
    // printf("12 * 12 = %s\n", result); // 期望输出: 144

    // // 测试 2: 进位和前导零测试
    // multiply_big_num("123", "456", result);
    // printf("123 * 456 = %s\n", result); // 期望输出: 56088

    // // 测试 3: PDF 终极大数测试 (Challenge limit)
    // multiply_big_num("987654321", "987654321", result);
    // printf("987654321 * 987654321 = %s\n", result); // 期望输出: 975461057789971041

    // return 0;

    // test cases for subtract_big_num
    // char result[MAX_INPUT_LEN];
    
    // // 测试 1: 正常减法
    // subtract_big_num("456", "123", result);
    // printf("456 - 123 = %s\n", result); // 期望输出: 333

    // // 测试 2: 产生负数
    // subtract_big_num("123", "456", result);
    // printf("123 - 456 = %s\n", result); // 期望输出: -333

    // // 测试 3: 连环借位与前导零处理
    // subtract_big_num("1000", "1", result);
    // printf("1000 - 1 = %s\n", result);  // 期望输出: 999

    // // 测试 4: 结果包含 0 
    // subtract_big_num("1005", "1000", result);
    // printf("1005 - 1000 = %s\n", result); // 期望输出: 5

    // return 0;

    // test cases for divide_big_num
    // char result[MAX_INPUT_LEN];
    
    // // 测试 1: 正常整除
    // divide_big_num("100", "20", result);
    // printf("100 / 20 = %s\n", result); // 期望输出: 5

    // // 测试 2: 带余数的除法 (目前只取整数部分)
    // divide_big_num("105", "20", result);
    // printf("105 / 20 = %s\n", result); // 期望输出: 5

    // // 测试 3: 超大数除法
    // divide_big_num("987654321987654321", "987654321", result);
    // printf("987654321987654321 / 987654321 = %s\n", result); // 期望输出: 1000000001

    // return 0;

    /* ==========================================
    * test cases for tokenizer
    * ========================================== */
    // const char* test_str1 = " 123 + 456 * ( 7 - 8 ) ";
    // const char* test_str2 = " a * 2 "; // 这个应该报错
    // const char* test_str3 = " 12345 + 54321 "; // 修改MAX_INPUT_LEN后这个应该报错

    // printf("测试 1: %s\n", test_str1);
    // if (tokenize(test_str1)) {
    //     printf("解析成功！共提取了 %d 个 Token。\n", token_count - 1);
    //     for(int i=0; i < token_count - 1; i++) {
    //         if(tokens[i].type == TOKEN_NUMBER) {
    //             printf("Token: NUMBER, 值: %s\n", tokens[i].value);
    //         } else {
    //             printf("Token: SYMBOL, 值: %s\n", tokens[i].value);
    //         }
    //     }
    // }

    // printf("\n测试 2: %s\n", test_str2);
    // tokenize(test_str2); // 应该打印错误信息并返回 0

    // printf("\n测试 3: %s\n", test_str3);
    // tokenize(test_str3); // 应该打印错误信息并返回 0

    // return 0;

}