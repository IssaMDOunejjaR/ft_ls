#include "../ft_ls.h"

int file_info_cmp_by_name(FileInfo *a, FileInfo *b) {
  return ft_strcmp(a->name, b->name);
}

int file_info_cmp_by_size(FileInfo *a, FileInfo *b) {
  long ret = b->stat->st_size - a->stat->st_size;

  if (ret < 0)
    return -1;
  else if (ret > 0)
    return 1;

  return file_info_cmp_by_name(a, b);
}

int file_info_cmp_by_update_time(FileInfo *a, FileInfo *b) {
  int64_t s1_time_ns =
      (a->stat->st_mtim.tv_sec * 1000000000LL) + a->stat->st_mtim.tv_nsec;
  int64_t s2_time_ns =
      (b->stat->st_mtim.tv_sec * 1000000000LL) + b->stat->st_mtim.tv_nsec;

  if (s1_time_ns < s2_time_ns)
    return 1;
  else if (s1_time_ns > s2_time_ns)
    return -1;

  return file_info_cmp_by_name(a, b);
}

int file_info_cmp_by_access_time(FileInfo *a, FileInfo *b) {
  int64_t s1_time_ns =
      (a->stat->st_atim.tv_sec * 1000000000LL) + a->stat->st_atim.tv_nsec;
  int64_t s2_time_ns =
      (b->stat->st_atim.tv_sec * 1000000000LL) + b->stat->st_atim.tv_nsec;

  if (s1_time_ns < s2_time_ns)
    return 1;
  else if (s1_time_ns > s2_time_ns)
    return -1;

  return file_info_cmp_by_name(a, b);
}

void sort_inputs(Input *input) {
  if (sort_type == sort_update_time)
    ft_list_sort(&input->list, (CompareFunc)&file_info_cmp_by_update_time);
  else if (sort_type == sort_access_time)
    ft_list_sort(&input->list, (CompareFunc)&file_info_cmp_by_access_time);
  else if (sort_type == sort_name)
    ft_list_sort(&input->list, (CompareFunc)&file_info_cmp_by_name);
  else if (sort_type == sort_size)
    ft_list_sort(&input->list, (CompareFunc)&file_info_cmp_by_size);

  if (sort_reverse)
    ft_list_reverse(&input->list);
}
