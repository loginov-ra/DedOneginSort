
#include "Text.h"
#include <getopt.h>

struct Options
{
    bool needOrig;
    bool needSort;
    bool needRev;

    const char* inputFilename;
    const char* outputFilename;
};

void printFiles(Text& text, FILE* output, bool needOrig = true, bool needSort = true, bool needRev = true)
{
    assert(text.isOk());
    assert(output);
    
    if (needSort)
    {
        text.sort();
        text.printToFile(output);
    }

    if (needRev)
    {
        text.sort(reverseStringComparator);
        text.printToFile(output);
    }

    if (needOrig)
    {
        text.recoverOriginal();
        text.printToFile(output);
    }
}

Options getOptions(int argc, char** argv);

int main(int argc, char** argv)
{
    Options options = getOptions(argc, argv);
    FILE* output = fopen(options.outputFilename, "wb");
    
    if (!output)
    {
        printf("Unable to open file %s for output\n", options.outputFilename);
        assert(output);
    }

    Text text(options.inputFilename);
    printFiles(text, output, options.needOrig, options.needSort, options.needRev);
    
    printf("Asked versions written to %s\n", options.outputFilename);

    fclose(output);
    return 0;
}

Options getOptions(int argc, char** argv)
{
    opterr = 1;
    Options options = { false, false, false, "", "output.txt" };
    
    const char* possibleOptions = "i:osr";
    option longOpt[6] = { {"input", 1, nullptr, 'i'},
                          {"original", 0, nullptr, 'o'},
                          {"sorted", 0, nullptr, 's'},
                          {"rev", 0, nullptr, 'r'},
                          {"output", 1, nullptr, 0},
                          {0, 0, 0, 0} };

    int opt = 0;
    int optionIndex = 0;
    while ((opt = getopt_long(argc, argv, possibleOptions, longOpt, &optionIndex)) != -1)
    {
        switch (opt)
        {
            case 'i':
                options.inputFilename = optarg;
                break;

            case 'o':
                options.needOrig = true;
                break;

            case 's':
                options.needSort = true;
                break;

            case 'r':
                options.needRev = true;
                break;

            case 0:
                if (longOpt[optionIndex].flag != 0)
                    break;
                if (strcmp(longOpt[optionIndex].name, "output") == 0)
                    options.outputFilename = optarg;
                break;
        }
    }
    
    if (options.needSort + options.needRev + options.needOrig == 0)
        options.needSort = options.needRev = options.needOrig = 1;

    return options;
}

