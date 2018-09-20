
/*!
 * \file
 * \brief
 * \details Interface for working with text files in UTF-16. Methods for quick line sorting are provided.
 * \author Roman Loginov
 * \version 1.0
 */

/*! \mainpage OneginSort
 *  \section Headers
 * Project has Text.h library. <br>
 * There you can work with only one buffer reproducing all file content, but with special objects, helping to separate text to lines <br>
 * Moreover, you are able to look at your file lexicographically
 *
 * \section Usage
 * Example of usage is sorting of "Eugene Onegin" poem <br>
 * Why not to look at "Russian life encyclopedia" in alphabetical order? <br>
 * Hope you will like this.
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cwchar>
#include <algorithm>
#include <endian.h>
#include <cmath>
#include <functional>
#include <cstring>

/*!
 * Newline character in UTF-16
 */
const char16_t UTF16_NEWLINE = char16_t(0xfeff000a);

/*!
 * Compares symbols in little-endian utf-16 lexicographically
 * @param c1, c2 2-bytr Unicode symbols
 * @return -1 if c1 < c2, 0 if c1 == c2, otherwise 1
 */
int utf16_comp_le(char16_t c1, char16_t c2)
{
    c1 = htobe16(c1);
    c2 = htobe16(c2);
    
    if (c1 < c2)
        return -1;
    else if (c1 == c2)
        return 0;
    else
        return 1;
}

/*!
 * strlen for UTF-16
 * @return Number of characters in str
 */
size_t utf16_strlen(const char16_t* str)
{
    size_t currentLen = 0;

    while (str[currentLen] != L'\0')
       currentLen += 1;

   return currentLen; 
}

/*!
 * std::string.count() analogue fo UTF-16
 * @param str String to find in
 * @param symbol Symbol to find
 * @return number of given symbols in str
 */ 
size_t utf16_count(const char16_t* str, char16_t symbol)
{
    size_t answer = 0;
    size_t i = 0;

    while (str[i] != L'\0')
    {
        answer += (str[i] == symbol);
        i += 1;
    }

    return answer;
}

/*!
 * Count bytes in file
 * @param filename Path to wanted file
 * @return Number of bytes in file, if it is found, 0 otherwise
 */
size_t getFileBytesNumber(const char* filename)
{
    struct stat st = {};

    if (stat(filename, &st) != 0)
        return 0;
    
    return st.st_size;
}

/*!
 * Get number of symbols in UTF-16 file
 * @param filename Path to wanted file
 * @return Number of symbols, if file is found, 0 otherwise
 */
size_t utf16_file_len(const char* filename)
{
    return getFileBytesNumber(filename) / sizeof(char16_t);
}

/*!
 * \brief String as a part of file
 *
 * String of a file with useful functions to work with <br>
 * Just a part of whole buffer, no use of dynamic memory
 */
class IntegratedString
{
public:
    const char16_t* ptr_; //!< Pointer to the beginning
    size_t size_;         //!< Length of line
    
    static constexpr const size_t    N_PROHIBITED_ = 11;              //!< Number of skipped symbols
    static constexpr const char16_t*   PROHIBITED_ = u".,!:;\"?-() "; //!< Skipped symbols
    
    /*!
     * Tell if symbol is service and should be skipped during a sort
     */
    bool isProhibitedSymbol(char16_t sym) const
    {
        for (size_t i = 0; i < N_PROHIBITED_; ++i)
            if (PROHIBITED_[i] == sym)
                return true;

        return false;    
    }
    
    /*!
     * \brief Directional UTF-16 string comparator
     * By choosing a direction goes from given starts and determines if <br>
     * resulting string is than given another got with the same way <br>
     * Skips service symbols
     * @param that String to compare with
     * @param startLHS, startRHS Place from where to start line formation
     * @param direction 1 for moving to the end of string, -1 otherwise
     * @result Result of operator < on resulted strings
     */
    bool directionalCompare(const IntegratedString& that, size_t startLHS = 0, size_t startRHS = 0, int direction = 1) const
    {
        assert(abs(direction) == 1);

        size_t indLHS = 0, indRHS = 0;

        while (indLHS < getSize() && indRHS < getSize())
        {
            if (isProhibitedSymbol(ptr_[startLHS + direction * indLHS]))
            {
                ++indLHS;
                continue;
            }
            
            if (isProhibitedSymbol(that.ptr_[startRHS + direction * indRHS]))
            {
                ++indRHS;
                continue;
            }

            int comp_res = utf16_comp_le(     ptr_[startLHS + direction * indLHS], 
                                         that.ptr_[startRHS + direction * indRHS]);
            ++indLHS;
            ++indRHS; 

            if (comp_res < 0)
                return true;
            else if (comp_res > 0)
                return false;
        } 
        
        while (indLHS < getSize() && isProhibitedSymbol(ptr_[startLHS + direction * indLHS]))
            ++indLHS;
        
        while (indRHS < that.getSize() && isProhibitedSymbol(that.ptr_[startRHS + direction * indRHS]))
            ++indRHS;

        return (indLHS >= getSize() && indRHS < that.getSize());
    }
    
public:
    IntegratedString():
        ptr_(nullptr),
        size_(0)
    {}
    
    /*!
     * Construct from pointer to the end <br>
     * Uses utf16_strlen(ptr)
     */
    explicit IntegratedString(const char16_t* ptr):
        ptr_(ptr),
        size_(utf16_strlen(ptr))
    {}

    IntegratedString(const char16_t* ptr, size_t size):
        ptr_(ptr),
        size_(size)
    {}
    
    /*!
     * Constant string getter
     */
    const char16_t* getPtr() const
    {
        return ptr_;
    }
    
    /*!
     * Get number of symbols in line
     */
    size_t getSize() const
    {
        return size_;
    }
    
    /*!
     * Check if buffer is correct
     */
    bool isOk() const
    {
        return ptr_ != nullptr;
    }
    
    /*!
     * Forward comparison operator
     * @see directionalCompare
     */
    bool operator <(const IntegratedString& that) const
    {
        return directionalCompare(that);       
    }
    
    /*!
     * Backward comparator
     * @see directionalCompare
     */
    bool compareReversed(const IntegratedString& that) const
    {
        return directionalCompare(that, getSize() - 1, that.getSize() - 1, -1);
    }
};

/*!
 * \brief Represents current order of lines
 * In fact closes vector order form user's eye
 */
class LineOrder
{
    friend class Text;

    private:
        std::vector<IntegratedString> lines_;

        LineOrder(size_t nLines, IntegratedString* strings_):
            lines_(strings_, strings_ + nLines)
        {}
};

/*!
 * Backward comparator in a form of not-a-member function
 * @see IntegratedString::compareReversed(that)
 */
bool reverseStringComparator(const IntegratedString& lhs, const IntegratedString& rhs)
{
    return lhs.compareReversed(rhs);
}

/*!
 * \brief Class to represent text from file
 * \author Roman Loginov
 *
 * Implementation for working with file as a whole buffer <br>
 * Provides interface for working with file order
 */
class Text
{
private:
    FILE* sourceFile_; //!> Source file
    char16_t* buffer_; //!> Place to read a whole file
    size_t nSymbols_;  //!> File size in symbols
    size_t nLines_;    //!> Number of lines in file

    IntegratedString* strings_;  //!> Current order of lines
    IntegratedString* original_; //!> Original order not to be killed
    
    /*!
     * Open file and read to already created bufer
     */
    bool readFile(const char* filename)
    {
        sourceFile_ = fopen(filename, "r");
        if (!sourceFile_)
            return false;

        assert(buffer_);
        return fread(buffer_, sizeof(char16_t), nSymbols_, sourceFile_) == nSymbols_; 
    }
    
    /*!
     * Separates buffer into lines <br>
     * Stores result in a form of InegratedString array
     */
    void separateBufferIntoLines()
    {
        nLines_ = utf16_count(buffer_, L'\n') + 1;
        strings_ = new IntegratedString[nLines_];
        
        size_t currLength = 0;
        size_t currLine = 0;
        char16_t* currBeginning = buffer_ + 1;
        
        
        for (size_t i = 1; i < nSymbols_; ++i)
        {
            if (buffer_[i] != L'\n')
            {
                ++currLength;
            }
            else
            {
                strings_[currLine] = IntegratedString(currBeginning, currLength);
                currBeginning = buffer_ + i + 1;
                currLength = 0;
                ++currLine;
                buffer_[i] = L'\0';
            }
        }

        strings_[currLine] = IntegratedString(currBeginning, currLength);
    }
    
    /*!
     * Removes blank lines from the end
     */
    void shrinkEmptyLines()
    {
        while (strings_[nLines_ - 1].getSize() == 0 && nLines_ > 0)
            --nLines_;
    }

public:
    /*!
     * Main constructor. <br>
     * Reads whole file and structurizes it
     */
    Text(const char* filename):
        sourceFile_(nullptr),
        buffer_(nullptr),
        nSymbols_(utf16_file_len(filename)),
        nLines_(0),
        strings_(nullptr)
    {
        buffer_ = new char16_t[nSymbols_ + 2];
        
        if (!readFile(filename))
        { 
            fprintf(stderr, "Unable to read file: %s\n", filename);
            return;
        }

        separateBufferIntoLines();
        shrinkEmptyLines();

        original_ = new IntegratedString[nLines_];
        memcpy(original_, strings_, nLines_ * sizeof(IntegratedString));
    }
    
    /*!
     * Prints contents in current order to provided file line by line
     * @param output File to print in
     */
    void printToFile(FILE* output) const
    {
        assert(output);
        fwrite(buffer_, sizeof(char16_t), 1, output);

        for (size_t i = 0; i < nLines_; ++i)
        {
            fwrite(strings_[i].getPtr(), sizeof(char16_t), strings_[i].getSize(), output); 
            fwrite(&UTF16_NEWLINE, sizeof(char16_t), 1, output);
        }
    }
    
    /*!
     * Checks pointers for correctness
     */
    bool isOk() const
    {
        return sourceFile_ && buffer_ && strings_;
    }
    
    /*!
     * Sorts lines in text with std::sort
     * @tparam Comparator - Comparator type for IntegratedStrings
     * @param comp - given type comparator
     */
    template <typename Comparator = std::less<IntegratedString>>
    void sort(Comparator comp = std::less<IntegratedString>())
    {
        std::sort(strings_, strings_ + nLines_, comp);
    }
    
    /*!
     * @return Current line order for futher usage
     */
    LineOrder getOrder()
    {
        return LineOrder(nLines_, strings_);
    }
    
    /*!
     * Set previously saved line order
     */
    void setOrder(const LineOrder& order)
    {
        assert(order.lines_.size() == nLines_);
        memcpy(strings_, order.lines_.data(), nLines_ * sizeof(IntegratedString));
    }
    
    /*!
     * Returns current line order to the original one
     */
    void recoverOriginal()
    {
        memcpy(strings_, original_, nLines_ * sizeof(IntegratedString));
    }

    ~Text()
    {
        if (buffer_)
        {
            delete[] buffer_;
            buffer_ = nullptr;
        }
        
        if (original_)
        {
            delete[] original_;
            original_ = nullptr;
        }

        if (strings_)
        {
            delete[] strings_;
            strings_ = nullptr;
        }

        if (sourceFile_)
        {
            fclose(sourceFile_);
            sourceFile_ = nullptr;
        }
    }
};
