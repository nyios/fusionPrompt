#include <stdint.h>
#include <string>

enum class TokenType : uint8_t {
    WORD,
    WHITESPACE,
    STRING,
    PIPE,
    ASSIGN,
    IO_IN,
    IO_OUT,
    IO_OUT_APPEND,
};

struct iotoken_t {
    uint8_t fd;
};

struct Token {
    union {
        std::string_view text;
        iotoken_t io;
    };
    TokenType type;
};

struct Command {
    //TODO
};

class Parser {
public:
    template <typename Iterator>
    Command parse(Iterator begin) {
        Iterator current = begin;
        
    }

private:
    void check_token_type(TokenType type, std::initializer_list<TokenType> allowed) {
        for (auto other : allowed) {
            if (other == type) {
                return;
            }
        }
        //TODO
        throw 1337;
    }

    template <typename Iterator, std::enable_if<std::is_same_v<std::remove_cvref_t<typename Iterator::value_type>, Token>>>
    void parse_preamble1(Iterator begin) {
        Token &token = *begin;
        check_token_type(token.type, {TokenType::WORD});
        parse_preamble2(++begin);
    }

    template <typename Iterator>
    void parse_preamble2(Iterator begin) {
        Token &token = *begin;
        switch(token.type) {
            case TokenType::WHITESPACE:
                CHECK(parse_command(begin));
            case TokenType::ASSIGN:
                parse_env(++begin);
                check_token_type(begin->type, {TokenType::WHITESPACE});
                parse_preamble1(++begin);
            default:
                check_token_type(token.type, {TokenType::WHITESPACE, TokenType::ASSIGN});
        }
    }
};
