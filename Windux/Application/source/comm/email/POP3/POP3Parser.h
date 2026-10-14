#ifndef POP3PARSER_H
#define POP3PARSER_H



#include <string>
#include <vector>



namespace email
{


	class POP3Parser
	{
		public:
			// --- Types and subtypes ---
			typedef std::string value_type;
			typedef unsigned int size_type;
			typedef int difference_type;

			class parserIterator
			{
				public:
					inline parserIterator(const POP3Parser* parser, POP3Parser::size_type begin, POP3Parser::size_type length)
							: parser_(parser), begin_(begin), length_(length), token_(parser_->getToken(begin_, length_)) {}
					inline parserIterator(const parserIterator& rhs)
							: parser_(rhs.parser_), begin_(rhs.begin_), length_(rhs.length_), token_(rhs.token_) {}
					inline ~parserIterator() {}

					inline parserIterator& operator =(const parserIterator& rhs)
							{ parser_ = rhs.parser_; begin_ = rhs.begin_; length_ = rhs.length_; return *this; }

					inline const std::string& operator *() const { return token_; }

					inline bool operator ==(const parserIterator& rhs) { return parser_ == rhs.parser_ && begin_ == rhs.begin_ && length_ == rhs.length_; }
					inline bool operator !=(const parserIterator& rhs) { return !(*this == rhs); }

					inline parserIterator& operator ++() { return *this; }
					inline const parserIterator& operator ++() const;
					inline parserIterator operator ++(int);
					inline const parserIterator operator ++(int) const;

					inline parserIterator& operator --();
					inline const parserIterator& operator --() const;
					inline parserIterator operator --(int);
					inline const parserIterator operator --(int) const;

				private:
					const POP3Parser* parser_;
					POP3Parser::size_type begin_;
					POP3Parser::size_type length_;
					std::string token_;
			};
			typedef parserIterator iterator;

			typedef parserIterator iterator;
			typedef const parserIterator const_iterator;
			typedef std::reverse_iterator<iterator> reverse_iterator;
			typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


			// --- Constructors and destructor ---
			POP3Parser(const std::string& text);
			~POP3Parser();


			// --- Getters and setters ---


			// --- Other function members ---
			parserIterator begin();
			parserIterator end();

			std::string getToken(size_type begin, size_type length) const { return text_.substr(begin, length); }


		protected:


		private:
			// --- Instance data members ---
			std::string text_;


			// --- Class data members ---


#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //POP3PARSER_H

