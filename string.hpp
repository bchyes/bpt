#ifndef SJTU_STRING_HPP
#define SJTU_STRING_HPP

#include <utility>

namespace sjtu {

    class string {
    private:
        char alpha[65];
        int len = 0;
    public:
        string() {
            alpha[0] = '\0';
        }

        int length() const {
            return len;
        }

        string(const std::string &other) {
            for (int i = 0; i < other.length(); i++)
                alpha[i] = other[i];
            alpha[other.length()] = '\0';
            len = other.length();
        }

        string(const string &other) {
            for (int i = 0; i < other.length(); i++)
                alpha[i] = other.alpha[i];
            alpha[other.length()] = '\0';
            len = other.length();
        }

        friend std::istream &operator>>(std::istream &is, string &s) {
            std::string s_;
            is >> s_;
            strcpy(s.alpha, s_.c_str());
            s.len = s_.length();
            return is;
        }

        bool operator==(const std::string &s_) {
            if (len != s_.length()) return 0;
            for (int i = 0; i < len; i++) {
                if (alpha[i] != s_[i]) return 0;
            }
            return 1;
        }

        bool operator<(const string &s_) const {
            if (strcmp(alpha, s_.alpha) < 0) return 1;
            else return 0;
        }

        char &operator[](const int &i) {
            return alpha[i];
        }

    };

}

#endif
