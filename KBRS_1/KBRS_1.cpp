// KBRS_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include <set>
#include<algorithm>
#include <regex>
using namespace std;

const int CAESAR_SHIFT = 3;
const string VIGENERE_WORD = "KEYWORD";
const vector<string> VIGENERE_WORDS = {
	"AS", "YOU", "KNOW", "THERE", "SHOULD", "NORMALLY", "BEAPERSON", "WHOWILLMAKE", "YOULAUGHOUTLOUD"
};
const int L_GRAMM = 7;
const double DINF = 1000000.0;


struct LangException : public exception {
	LangException(char c, const string& msg) {
		e_msg = "Language Exception: ";
		e_msg += c;
		e_msg += ' ';
		e_msg += msg;
	}
	LangException(const string& msg) {
		e_msg = msg;
	}
	virtual const char* what() const throw() {
		return e_msg.c_str();
	}
private: 
	string e_msg;
};

struct LanguageParams {
	int n;  // Total number of characters.
	char first_low;  // First low character of the alphabet.
	char first_high;  // First high character of the alphabet.
	char most_freq_char;
	regex reg;  // Alphabet regex.
	vector<double> fr;  // Frequences of the letters.
};

const map<string, LanguageParams> langs = {
	make_pair("en", LanguageParams{26, 'a', 'A', 'e', regex("^[a-zA-Z]+$"), 
		{0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.0228, 0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749, 0.07507, 0.01929, 0.00095, 0.05987, 0.06327, 0.09056, 0.02758, 0.00978, 0.0236, 0.0015, 0.01974, 0.00074}
		}),
	make_pair("ru", LanguageParams{32, 'а', 'А', 'о', regex("^[а-яА-Я]+$"), 
		{0.07821, 0.01732, 0.04491, 0.01698, 0.03103, 0.08567, 0.01082, 0.01647, 0.06777, 0.01041, 0.03215, 0.04813, 0.03139, 0.0685, 0.11394, 0.02754, 0.04234, 0.05382, 0.06443, 0.02882, 0.00132, 0.00833, 0.00333, 0.01645, 0.00775, 0.00331, 0.00023, 0.01854, 0.02106, 0.0031, 0.00544, 0.01979}
		})
};

void output(string msg, const vector<string>& str) {
	cout << msg << ": ";
	for (auto s : str)
		cout << s << ' ';
	cout << endl;
}

template <class T>
void output(string msg, T ans) {
	cout << msg << ' ' << ans << endl;
}

int get_gcd(int a, int b) {
	while (a > 0 & b > 0) {
		if (a > b)
			a %= b;
		else
			b %= a;
	}
	return a + b;
}

int get_gcd(const set<int>& dist) {
	int gcd = *dist.begin();
	for (auto d : dist) {
		gcd = get_gcd(gcd, d);
	}
	return gcd;
}

bool is_stop(const LanguageParams& lp, char c) {
	std::smatch base_match;
	std::basic_string<char> text(&c);
	return (!regex_match(text, base_match, lp.reg));
}

bool is_high(const LanguageParams& lp, char c) {
	std::smatch base_match;
	std::basic_string<char> text(&c);
	return (c < lp.first_low || c > lp.first_low + lp.n - 1);
}

int get_shift(const LanguageParams& lp, char c) {
	return (is_high(lp, c)) ? (c - lp.first_high) : (c - lp.first_low);
}

char get_char_by_shift(const LanguageParams& lp, int shift, bool get_high = false) {
	if (get_high)
		return static_cast<char>(lp.first_high + shift);
	else
		return static_cast<char>(lp.first_low + shift);
}

char char_cipher(char c, const LanguageParams& lp, bool is_decode, int shift) {
	if (is_high(lp, c))
		return lp.first_high + ((c - lp.first_high) + lp.n + (is_decode ? -1 : 1) * shift) % lp.n;
	else 
	    return lp.first_low + ((c - lp.first_low) + lp.n + (is_decode ? -1 : 1) * shift) % lp.n;
}

void Caesar(const vector<string> & input, vector<string> & encode, const LanguageParams& lp,  bool is_decode) {
	for (auto s : input) {
		string str ="";
		for (auto c : s) {
			if (is_stop(lp, c))
				str += c;
			else
			    str += char_cipher(c, lp, is_decode, CAESAR_SHIFT);
		}
		encode.push_back(str);
	}
}

void Vigenere(const vector<string> & input, vector<string> & encode, const LanguageParams& lp, bool is_decode) {
	int word_uk = 0;
	for (auto s : input) {
		string str = "";
		for (auto c : s) {
			if (is_stop(lp, c)) {
				str += c;
			}
			else {
				str += char_cipher(c, lp, is_decode, get_shift(lp, VIGENERE_WORD[word_uk]));
				word_uk = (word_uk + 1) % VIGENERE_WORD.size();
			}
		}
		encode.push_back(str);
	}
}

void get_distances(const string& str, set<int>& dist, int l) {
	for (size_t i = 0; i < str.size() - l; i++) {
		string sub = str.substr(i, l);
		size_t position = i; // holds the last occured position of the considered substr

		size_t pos = str.find(sub, i + l);
		while (pos != string::npos)
		{
			dist.insert(pos - position);
			position = pos;
			pos = str.find(sub, pos + l);
		}
	}
}

int Kasiski(const vector<string>& encode, const LanguageParams& lp, int l) {
	string str = "";
	for (auto s : encode) {
		for (auto c : s) {
			if (!is_stop(lp, c))
				str += c;
		}
	}
	set<int> dist;
	get_distances(str, dist, l);
	if (!dist.size()) throw LangException("L_GRAMM parameter is too big. No repetitions were found.");
	return get_gcd(dist);
}

string FreqAnalyzer(const vector<string>& encode, const LanguageParams& lp, int word_length) {
	string word = "";
	for (size_t i = 0; i < word_length; i++) {
		vector<int> nums;
		nums.resize(lp.n, 0);
		int total_length = 0;
		for (auto s : encode) {
			for (auto c : s) {
				if (is_stop(lp, c)) continue;
				if (total_length % word_length == i) {
					nums[get_shift(lp, c)]++;
				}
				total_length++;
			}
		}

		// Frequency analysis.
		{
			int min_shift = -1;
			double min_diff = DINF;
			for (int shift = 0; shift < lp.n; shift++) {
				double cur_diff = 0.0;
				for (int i = 0; i < lp.n; i++) {
					cur_diff += fabs((1.0 * nums[i] / total_length) - lp.fr[(i + shift) % lp.n]);
				}
				if (cur_diff < min_diff) {
					min_diff = cur_diff;
					min_shift = shift;
				}
			}
			word += get_char_by_shift(lp, (lp.n - min_shift) % lp.n, /*get_high=*/true);
		}
	}
	return word;
}

void RunCaesarCryptoCycle(const vector<string>& input, const LanguageParams& lp) {
	vector<string> encoded_text;
	vector<string> decoded_text;

	// -----Caesar cipher-----
	try {
		// encode Input text with Caesar cipher using SHIFT param
		Caesar(input, encoded_text, lp, /*is_decode=*/false);

		// Decode Input text with Caesar cipher using CAESAR_SHIFT param
		Caesar(encoded_text, decoded_text, lp, /*is_decode=*/true);
		output("input text", input);
		output("Caesar encoded text", encoded_text);
		output("Caesar decoded text", decoded_text);
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

}

// -----Vigenere cipher -> Kasiski test -> Text analyzer-----
string RunVigenereCryptoCycle(const vector<string>& input, const LanguageParams& lp) {
	vector<string> encoded_text;
	vector<string> decoded_text;
	try {
		// encode Input text with Vigenere cipher using VIGENERE_WORD as a key
		Vigenere(input, encoded_text, lp, /*is_decode=*/false);

		// Decode Input text with Vigenere cipher using VIGENERE_WORD as a key
		//Vigenere(encoded_text, decoded_text, lp, /*is_decode=*/true);
		//output("input text", input);
		//output("Vigenere encoded text", encoded_text);
		//output("Vigenere decoded text", decoded_text);

		// Use Kasiski test to find key word length
		int word_length = Kasiski(encoded_text, lp, L_GRAMM);
		//output("Key word length according to Kasiski test is", word_length);
		string key_word = FreqAnalyzer(encoded_text, lp, word_length);
		//output("Key word according to frequency analyzer is", key_word);
		return key_word;
	}
	catch (exception& e) {
		//cout << e.what() << endl;
	}
	return "";
}

void TextLengthExp(const vector<string>& complete_input, const LanguageParams& lp) {
	FILE* fp = freopen("en_len_out.txt", "w", stdout);

	for (int text = 300; text < 9000; text += 500) {   // Approximate text lengths
		int final_text_ln = 0;
		int final_freq = 0;
		const int _reps = 5;
		for (int q = 0; q < _reps; q++) {  // Number of repetitions per text length
			vector<string> input;
			int un = rand() % 500;
			int text_ln = 0;
			while (text_ln < text && un < complete_input.size()) {
				input.push_back(complete_input[un]);
				text_ln += complete_input[un++].length();
			}
			string key_word = RunVigenereCryptoCycle(input, lp);
			final_text_ln += text_ln;
			final_freq += (key_word == VIGENERE_WORD);
		}
		cout << final_text_ln / _reps << ' ' << (100 * (1.0 * final_freq / _reps)) << endl;
	}
	
	fclose(fp);
}

void KeyLengthExp(const vector<string>& complete_input, const LanguageParams& lp) {
	FILE* fp = freopen("en_key_out.txt", "w", stdout);

	for (auto vigenere_word : VIGENERE_WORDS) {   // L_gramm length
		int final_text_ln = 0;
		int final_freq = 0;
		const int _reps = 5;
		const int text = 3000;
		for (int q = 0; q < _reps; q++) {  // Number of repetitions per L_gramm length
			vector<string> input;
			int un = rand() % 500;
			int text_ln = 0;
			while (text_ln < text && un < complete_input.size()) {
				input.push_back(complete_input[un]);
				text_ln += complete_input[un++].length();
			}
			string key_word = RunVigenereCryptoCycle(input, lp);
			final_text_ln += text_ln;
			final_freq += (key_word == VIGENERE_WORD);
		}
		cout << final_text_ln / _reps << ' ' << l_gramm << ' ' << (100 * (1.0 * final_freq / _reps)) << endl;
	}

	fclose(fp);
}


int main(int argc, const char * argv[]) {

	freopen("eng_text.txt", "r", stdin);
	vector<string> complete_input;
	string lang;
	cin >> lang;
	cout << lang << endl;
	if (langs.find(lang) == langs.cend()) {
		cout << "Language not found. Try again.\n";
		return 0;
	}
	
	LanguageParams lp = langs.find(lang)->second;
	string s;
	while (cin >> s) complete_input.push_back(s);

	//TextLengthExp(complete_input, lp);
	KeyLengthExp(complete_input, lp);
	return 0;
}
