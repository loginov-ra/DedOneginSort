
#include "RLTest.h"
#include "Text.h"
#include <cstring>
#include <string>
#include <fstream>
#include <algorithm>

const size_t TEST_STR_SIZE = 256;

size_t getNonEmptyLinesCount(const char* filename)
{ 
    size_t numberOfLines = 0;
    std::ifstream file(filename);
	std::string line;	

    while (std::getline(file, line))
        numberOfLines += (line.size() > 1);
    
    return numberOfLines;
}

DEFINE_TEST(StrlenCorrectness)
    size_t size = utf16_strlen(u"Тесты - порождение дьявола");
    size_t actualSize = 26;
    ASSERT_EQUAL(size, actualSize);
}

DEFINE_TEST(ComparatorCorrectRussian)
    IntegratedString putin(u"Путин");
    IntegratedString obama(u"Обама");
    ASSERT_TRUE(obama < putin);
    ASSERT_TRUE(reverseStringComparator(obama, putin)); //Obama is both-sized less
}

DEFINE_TEST(RewriteFile)
    const char*  inputFilename = "../TEST.txt";
    const char* outputFilename = "output.txt";
    FILE* output = fopen(outputFilename, "w");
    
    {
        Text text(inputFilename);
        ASSERT_TRUE(text.isOk());
        LineOrder order = text.getOrder();
        text.setOrder(order);
        text.printToFile(output);
    }
    
    fclose(output);
    system("diff ../TEST.txt output.txt > res");
    ASSERT_EQUAL(getFileBytesNumber("res"), 0);
}

DEFINE_TEST(CheckProhibitedSymbols)
	IntegratedString prohibited(u";.()!?!?!?!?!))))\".:");
	IntegratedString empty(u"");
	
	ASSERT_TRUE(!(prohibited < empty))
	ASSERT_TRUE(!(empty < prohibited))
}

DEFINE_TEST(CheckSameLenSorted)
    const char*  inputFilename = "../TEST.txt";
    const char* outputFilename = "output.txt";
    FILE* output = fopen(outputFilename, "w");

    {
        Text text(inputFilename);
        ASSERT_TRUE(text.isOk());
        text.sort();
        text.printToFile(output);
    }

    fclose(output);
    ASSERT_EQUAL(getNonEmptyLinesCount(inputFilename),
                 getNonEmptyLinesCount(outputFilename));
}

int main()
{
    RUN_TEST(StrlenCorrectness);
    RUN_TEST(ComparatorCorrectRussian);
    RUN_TEST(RewriteFile);
	RUN_TEST(CheckProhibitedSymbols);
    RUN_TEST(CheckSameLenSorted);
}
