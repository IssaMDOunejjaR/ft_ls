#include "../ft_ls.h"

void clear_file_info(FileInfo *file_info) {
  free(file_info->name);
  free(file_info->fullname);
  free(file_info->stat);

  free(file_info);
}

void clear_inputs(void) {
  ft_list_clear(all.list, (ClearFunc)&clear_file_info);
  ft_list_clear(files.list, (ClearFunc)&clear_file_info);
  ft_list_clear(dirs.list, (ClearFunc)&clear_file_info);

  while (group_cache != NULL) {
    free(group_cache->group_name);
    GroupCache *tmp = group_cache;
    group_cache = group_cache->next;
    free(tmp);
  }

  while (owner_cache != NULL) {
    free(owner_cache->owner_name);
    OwnerCache *tmp = owner_cache;
    owner_cache = owner_cache->next;
    free(tmp);
  }
}

void clear_column_info(ColumnInfo *column_info) {
  free(column_info->col_widths);
}
