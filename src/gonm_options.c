#include <err.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "gonm_options.h"
#include "gonm_master.h"
#include "gonm_help.h"

void gonm_options_process(int argc, char* argv[], struct gonm_master_args* master_args)
{
    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"address", required_argument, NULL, 'a'},
        {"port", required_argument, NULL, 'p'},
        {"backlog", required_argument, NULL, 'b'},
        {"worker-count", required_argument, NULL, 'w'},
        {"module-args", required_argument, NULL, 'm'},
        {NULL, 0, NULL, 0}
    };

    struct gonm_module_args* module_args;
    struct gonm_module_arg* module_arg;
    char* module_args_string;
    char* module_path_string;
    char* module_arg_string;
    char* module_arg_name_or_value;
    for(char option_char; option_char != -1; option_char = getopt_long(argc, argv, "ha:p:b:w:m:", options, NULL))
    {
        switch(option_char)
        {
        case 'h':
            gonm_help_print();
            exit(EXIT_SUCCESS);
            break;
        case 'a':
            master_args->address = optarg;
            break;
        case 'p':
            master_args->port = strtol(optarg, NULL, 10);
            break;
        case 'b':
            master_args->backlog = strtol(optarg, NULL, 10);
            break;
        case 'w':
            master_args->worker_count = strtol(optarg, NULL, 10);
            break;
        case 'm':
            module_args = malloc(sizeof(struct gonm_module_args));
            GONC_LIST_INIT(module_args);
            module_args_string = strdup(optarg);
            for(size_t i = 0; (module_arg_string = strsep(&module_args_string, ",")) != NULL; ++i)
            {
                if(i == 0)
                {
                    if((module_path_string = realpath(module_arg_string, NULL)) == NULL)
                        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
                    GONC_STRING_SET(&module_args->module_path, module_path_string);
                    continue;
                }
                module_arg = malloc(sizeof(struct gonm_module_arg));
                GONC_LIST_APPEND(module_args, module_arg);
                module_arg_string = strdup(module_arg_string);
                for(size_t j = 0; j != 2 && (module_arg_name_or_value = strsep(&module_arg_string, ":")) != NULL; ++j)
                {
                    if(j == 0)
                        GONC_STRING_SET(&module_arg->name, strdup(module_arg_name_or_value));
                    else
                        GONC_STRING_SET(&module_arg->value, strdup(module_arg_name_or_value));
                }
                free(module_arg_string);
            }
            free(module_args_string);
            GONC_LIST_APPEND(master_args->module_args_list, module_args);
            break;
        }
    }
}
