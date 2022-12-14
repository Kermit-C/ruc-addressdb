#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "client.h"

void show_usage()
{
    printf(
        "Usage: addressdb [options]\n\n"
        "Options:\n"
        "  -p, --path=<data path>           The path the data locates in.\n"
        "  -m, --metapath=<metadata path>   The path the metadata locates in.\n"
        "  -h, --help                       Show usage.\n\n");
}

int main(int argc, char **argv)
{
    char *PATH = "./data";
    char *META_PATH = "./meta";

    // 处理参数
    int arg_c;
    while (1)
    {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"path", required_argument, 0, 'p'},
            {"metapath", required_argument, 0, 'm'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}};

        arg_c = getopt_long(argc, argv, "p:m:h",
                            long_options, &option_index);
        if (arg_c == -1)
            break;

        switch (arg_c)
        {
        case 'p':
            PATH = optarg;
            break;

        case 'm':
            META_PATH = optarg;
            break;

        case 'h':
            show_usage();
            exit(0);
            break;

        case '?':
            printf("Command args wrong. Use --help to checkout.\n");
            exit(1);
            break;

        default:
            printf("Command args wrong. Use --help to checkout.\n");
            exit(1);
        }
    }
    if (optind < argc)
    {
        printf("Command args wrong. Use --help to checkout.\n");
        exit(1);
    }

    // 主程序
    printf(
        "#########################\n"
        "#   Kermit Address DB   #\n"
        "#########################\n\n");

    client_worker(PATH, META_PATH);

    return 0;
}
