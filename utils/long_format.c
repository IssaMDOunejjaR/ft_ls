#include "../ft_ls.h"

static char *get_permission(bool condition, char *value) {
  return condition ? value : "-";
}

static char *get_time(FileInfo *file_info) {
  time_t now, file_time;

#ifdef __APPLE__
  if (print_access_time) {
    file_time = file_info->stat->st_atimespec.tv_sec +
                (file_info->stat->st_atimespec.tv_nsec / 1000000000LL);
  } else {
    file_time = file_info->stat->st_mtimespec.tv_sec +
                (file_info->stat->st_mtimespec.tv_nsec / 1000000000LL);
  }
#endif /* ifdef __APPLE__ */

#ifdef __linux
  if (print_access_time) {
    file_time = file_info->stat->st_atim.tv_sec +
                (file_info->stat->st_atim.tv_nsec / 1000000000LL);
  } else {
    file_time = file_info->stat->st_mtim.tv_sec +
                (file_info->stat->st_mtim.tv_nsec / 1000000000LL);
  }
#endif /* ifdef __linux */

  char *time_str = ctime(&file_time);
  char *ret = NULL;

  double six_months_in_seconds = 182.5 * 24 * 60 * 60;

  time(&now);

  if (file_time > now - six_months_in_seconds) {
    ret = ft_substr(time_str, ft_index_of(time_str, ' ') + 1,
                    ft_last_index_of(time_str, ':') - 1);
  } else {
    char *year = ft_substr(time_str, ft_last_index_of(time_str, ' '),
                           ft_strlen(time_str) - 2);

    char *tmp = ft_substr(time_str, ft_index_of(time_str, ' ') + 1,
                          ft_index_of(time_str, ':') - 3);

    ret = ft_strjoin(tmp, year);

    free(year);
    free(tmp);
  }

  return ret;
}

// TODO: improve
void out_long_format(Input *input) {
  List *list = input->list;
  ColumnInfo column_info = input->column_info;
  size_t capacity = 256;
  char out[capacity];

  while (list != NULL) {
    size_t pos = 0;
    FileInfo *file_info = list->data;

    char temp[2] = {filetype_letter[file_info->filetype], '\0'};

    output_buffering(out, &pos, capacity, temp);

    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IRUSR, "r"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IWUSR, "w"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IEXEC, "x"));

    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IRGRP, "r"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IWGRP, "w"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IXGRP, "x"));

    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IROTH, "r"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IWOTH, "w"));

    if ((file_info->stat->st_mode & S_ISVTX))
      output_buffering(out, &pos, capacity,
                       get_permission(file_info->stat->st_mode & S_IXOTH, "t"));
    else
      output_buffering(out, &pos, capacity,
                       get_permission(file_info->stat->st_mode & S_IXOTH, "x"));

    if (input->column_info.print_acl) {
      if (file_info->print_attr)
        output_buffering(out, &pos, capacity, "@");
      else if (file_info->print_acl)
        output_buffering(out, &pos, capacity, "+");
      else
        output_buffering(out, &pos, capacity, " ");
    }

    output_buffering(out, &pos, capacity, " ");

    int size = column_info.nlink_width -
               ft_unsigned_number_len(file_info->stat->st_nlink);

    while (size > 0) {
      output_buffering(out, &pos, capacity, " ");

      size--;
    }

    char *number = ft_unsigned_num_to_str(file_info->stat->st_nlink);
    output_buffering(out, &pos, capacity, number);
    free(number);

    output_buffering(out, &pos, capacity, " ");

    if (print_owner) {
      if (numeric_ids) {
        size = column_info.owner_width -
               ft_unsigned_number_len(file_info->stat->st_uid);

        while (size > 0) {
          output_buffering(out, &pos, capacity, " ");

          size--;
        }

        char *uid = ft_itoa(file_info->stat->st_uid);
        output_buffering(out, &pos, capacity, uid);
        free(uid);
      } else {
        output_buffering(out, &pos, capacity, file_info->owner_name);

        size = column_info.owner_width - ft_strlen(file_info->owner_name);

        while (size > 0) {
          output_buffering(out, &pos, capacity, " ");

          size--;
        }
      }

      output_buffering(out, &pos, capacity, " ");
    }

    if (print_group) {
      if (numeric_ids) {
        size = column_info.group_width -
               ft_unsigned_number_len(file_info->stat->st_gid);

        while (size > 0) {
          output_buffering(out, &pos, capacity, " ");

          size--;
        }
        char *gid = ft_itoa(file_info->stat->st_gid);
        output_buffering(out, &pos, capacity, gid);
        free(gid);
      } else {
        output_buffering(out, &pos, capacity, file_info->group_name);

        size = column_info.group_width - ft_strlen(file_info->group_name);

        while (size > 0) {
          output_buffering(out, &pos, capacity, " ");

          size--;
        }
      }

      output_buffering(out, &pos, capacity, " ");
    }

    size =
        column_info.size_width - ft_signed_number_len(file_info->stat->st_size);

    while (size > 0) {
      output_buffering(out, &pos, capacity, " ");

      size--;
    }

    number = ft_signed_num_to_str(file_info->stat->st_size);
    output_buffering(out, &pos, capacity, number);
    free(number);

    output_buffering(out, &pos, capacity, " ");

    char *time = get_time(file_info);

    if (time != NULL) {
      size = 12 - ft_strlen(time);

      while (size > 0) {
        output_buffering(out, &pos, capacity, " ");

        size--;
      }

      output_buffering(out, &pos, capacity, time);
      free(time);
    }

    output_buffering(out, &pos, capacity, " ");

    if (print_quotes && column_info.print_quote && !file_info->print_quote)
      output_buffering(out, &pos, capacity, " ");

    if (print_with_color) {
      if ((file_info->stat->st_mode & S_ISVTX)) {
        output_buffering(out, &pos, capacity, BLACK);
        output_buffering(out, &pos, capacity, STICKY_BIT_COLOR);
      } else
        output_buffering(out, &pos, capacity,
                         filetype_color[file_info->filetype]);
    }

    if (print_quotes && file_info->print_quote) {
      if (file_info->has_single_quote)
        output_buffering(out, &pos, capacity, "\"");
      else
        output_buffering(out, &pos, capacity, "'");

      output_buffering(out, &pos, capacity, file_info->name);

      if (file_info->has_single_quote)
        output_buffering(out, &pos, capacity, "\"");
      else
        output_buffering(out, &pos, capacity, "'");
    } else
      output_buffering(out, &pos, capacity, file_info->name);

    if (print_with_color) {
      if ((file_info->stat->st_mode & S_ISVTX))
        output_buffering(out, &pos, capacity, RESET_BACK);

      output_buffering(out, &pos, capacity, WHITE);
    }

    if (write_slash && file_info->filetype == directory)
      output_buffering(out, &pos, capacity, "/");

    while (size > 0) {
      output_buffering(out, &pos, capacity, " ");

      size--;
    }

    if (file_info->filetype == symbolic_link) {
      output_buffering(out, &pos, capacity, " -> ");

      char l_name[256];
      ssize_t size = readlink(file_info->fullname, l_name, sizeof(l_name) - 1);

      if (size != -1) {
        l_name[size] = '\0';

        output_buffering(out, &pos, capacity, l_name);
      }
    }

    output_buffering(out, &pos, capacity, "\n");

    out[pos] = '\0';

    ft_putstr(1, out);

    list = list->next;
  }
}

void update_column_info(FileInfo *info, ColumnInfo *column_info) {
  column_info->block_size += info->stat->st_blocks;
  char *name = info->fullname ? info->fullname : info->name;

  if (info->filetype == symbolic_link) {
    char target[1024];
    ssize_t len = readlink(name, target, sizeof(target) - 1);

    if (len == -1) {
      perror("readlink");
    } else {
      target[len] = '\0';
      name = target;
    }
  }

#ifdef __APPLE__
  ssize_t acl_ret = getxattr(name, "com.apple.acl.text", NULL, 0, 0, 0);
  ssize_t attr_ret = listxattr(name, NULL, 0, 0);
#endif /* ifdef __APPLE__ */

#ifdef __linux__
  ssize_t acl_ret = getxattr(name, "system.posix_acl_access", NULL, 0);
  ssize_t attr_ret = listxattr(name, NULL, 0);
#endif /* ifdef __linux__ */

  if (acl_ret >= 0) {
    column_info->print_acl = true;
    info->print_acl = true;
  } else
    info->print_acl = false;

  if (attr_ret > 0) {
    column_info->print_acl = true;
    info->print_attr = true;
  } else
    info->print_attr = false;

  if (print_owner) {
    if (numeric_ids) {
      size_t owner_len = ft_unsigned_number_len(info->stat->st_uid);

      if (owner_len > column_info->owner_width)
        column_info->owner_width = owner_len;
    } else {
      char *owner = get_owner_name(info->stat->st_uid);

      if (owner) {
        info->owner_name = owner;
        size_t owner_len = ft_strlen(owner);

        if (owner_len > column_info->owner_width)
          column_info->owner_width = owner_len;
      }
    }
  }

  if (print_group) {
    if (numeric_ids) {
      size_t group_len = ft_unsigned_number_len(info->stat->st_gid);

      if (group_len > column_info->group_width)
        column_info->group_width = group_len;
    } else {
      char *group_name = get_group_name(info->stat->st_gid);

      if (group_name) {
        info->group_name = group_name;
        size_t group_len = ft_strlen(group_name);

        if (group_len > column_info->group_width)
          column_info->group_width = group_len;
      }
    }
  }

  size_t len = ft_unsigned_number_len(info->stat->st_nlink);

  if (len > column_info->nlink_width)
    column_info->nlink_width = len;

  size_t size_len = ft_signed_number_len(info->stat->st_size);

  if (size_len > column_info->size_width)
    column_info->size_width = size_len;

  char *has_set = ft_has_set(info->name, name_set);

  if (has_set != NULL) {
    if (!column_info->print_quote)
      column_info->print_quote = true;

    info->print_quote = true;

    if (has_set[0] == '\'')
      info->has_single_quote = true;
    else
      info->has_single_quote = false;

  } else
    info->print_quote = false;
}

void calc_long_format(Input *input) {
  List *list = input->list;
  ColumnInfo *column_info = &input->column_info;

  while (list != NULL) {
    FileInfo *file_info = list->data;

    update_column_info(file_info, column_info);

    list = list->next;
  }
}
