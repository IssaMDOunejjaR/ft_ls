#include "../ft_ls.h"

static void separate_input(void) {
  List *list = all.list;

  while (list != NULL) {
    List *tmp = list;
    FileInfo *file_info = list->data;

    if (file_info->filetype == symbolic_link) {
      char *name = file_info->fullname ? file_info->fullname : file_info->name;
      struct stat stats;

      if (stat(name, &stats) == -1) {
        perror("stat");
      } else {
        if (S_ISDIR(stats.st_mode)) {
          ft_list_push_back(&dirs.list, file_info);
          dirs.size++;
        } else {
          ft_list_push_back(&files.list, file_info);
          files.size++;
        }
      }
    } else if (file_info->filetype == normal ||
               file_info->filetype == executable) {
      ft_list_push_back(&files.list, file_info);
      files.size++;
    } else if (file_info->filetype == directory) {
      ft_list_push_back(&dirs.list, file_info);
      dirs.size++;
    }

    list = list->next;
    free(tmp);
  }

  all.list = NULL;
}

static void process_dir_content(FileInfo *file_info, int depth) {
  char *name = file_info
                   ? file_info->fullname ? file_info->fullname : file_info->name
                   : ".";

  DIR *o_dir = opendir(name);

  if (!o_dir) {
    print_error(name, "cannot open directory");
    return;
  }

  if (recursive) {
    if (depth > 0)
      ft_putchar(1, '\n');
    depth++;

    print_current_dir_name(name);
  }

  size_t name_len = ft_strlen(name);
  char full_path[name_len + 256];

  ft_strcpy(full_path, name);

  if (full_path[name_len - 1] != '/')
    full_path[name_len++] = '/';

  struct dirent *content;
  Input input;

  input.list = NULL;
  input.size = 0;

  init_column_info(&input.column_info);

  while ((content = readdir(o_dir))) {
    if (ignore_hidden_files && content->d_name[0] == '.')
      continue;

    if (ignore_dots && (ft_strcmp(".", content->d_name) == 0 ||
                        ft_strcmp("..", content->d_name) == 0))
      continue;

    ft_strcpy(full_path + name_len, content->d_name);

    struct stat *stat = malloc(sizeof(struct stat));

    if (lstat(full_path, stat) == -1) {
      perror("lstat");
      continue;
    }

    FileInfo *info = create_file_info(ft_strdup(content->d_name), stat);
    info->fullname = ft_strdup(full_path);
    update_column_info(info, &input.column_info);

    ft_list_push_back(&input.list, info);
    input.size++;
  }

  sort_inputs(&input);

  if (format == long_format) {
    print_block_size(input.column_info.block_size);
    out_long_format(&input);
  } else if (format == many_per_line) {
    FileInfo **list = input_list_to_table(input);

    calc_many_per_line_format(&input, list);
    out_column_format(&input, list);

    clear_column_info(&input.column_info);
    free(list);
  }

  if (recursive) {
    List *list = input.list;

    while (list != NULL) {
      FileInfo *file_info = list->data;

      if (!ignore_hidden_files) {
        if (ft_strcmp(".", file_info->name) == 0 ||
            ft_strcmp("..", file_info->name) == 0) {
          list = list->next;

          continue;
        }
      }

      if (file_info->filetype == directory)
        process_dir_content(file_info, depth);

      list = list->next;
    }
  }

  ft_list_clear(input.list, (ClearFunc)&clear_file_info);
  closedir(o_dir);
}

static void separate_process(void) {
  separate_input();
  sort_inputs(&files);
  sort_inputs(&dirs);

  if (files.size > 0) {
    print_out_format(files);

    if (dirs.size > 0 || all.error)
      ft_putchar(1, '\n');
  }

  if (dirs.size > 0) {
    List *list = dirs.list;

    while (list != NULL) {
      FileInfo *info = list->data;

      if (dirs.size > 1 || files.size > 0)
        print_current_dir_name(info->name);

      process_dir_content(info, 0);

      if (dirs.size > 1 && list->next != NULL)
        ft_putchar(1, '\n');

      list = list->next;
    }
  } else if (files.size == 0)
    process_dir_content(NULL, 0);
}

void process_inputs(void) {
  if (all.size == 0 && all.error)
    exit_status = 2;
  else if (immediate_dirs)
    print_out_format(all);
  else if (all.size == 0)
    process_dir_content(NULL, 0);
  else
    separate_process();
}
