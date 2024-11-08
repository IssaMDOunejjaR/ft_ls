#include "../ft_ls.h"

void output_buffering(char *arr, size_t *pos, size_t capacity, char *str) {
  size_t len = ft_strlen(str);

  while (len > 0) {
    size_t space_left = capacity - *pos - 1;

    if (len > space_left) {
      ft_strncpy(arr + *pos, str, space_left);
      *pos += space_left;
      arr[*pos] = '\0';
      write(1, arr, *pos);
      *pos = 0;
      str += space_left;
      len -= space_left;
    } else {
      ft_strcpy(arr + *pos, str);
      *pos += len;
      break;
    }
  }
}

char *get_owner_name(uid_t uid) {
  OwnerCache *current = owner_cache;

  while (current != NULL) {
    if (current->uid == uid)
      return current->owner_name;
    current = current->next;
  }

  struct passwd *owner = getpwuid(uid);
  if (!owner)
    return NULL;

  OwnerCache *new_cache = malloc(sizeof(OwnerCache));
  new_cache->uid = uid;
  new_cache->owner_name = ft_strdup(owner->pw_name);
  new_cache->next = owner_cache;
  owner_cache = new_cache;

  return new_cache->owner_name;
}

char *get_group_name(gid_t gid) {
  GroupCache *current = group_cache;

  while (current != NULL) {
    if (current->gid == gid)
      return current->group_name;
    current = current->next;
  }

  struct group *grp = getgrgid(gid);

  if (!grp)
    return NULL;

  GroupCache *new_cache = malloc(sizeof(GroupCache));
  new_cache->gid = gid;
  new_cache->group_name = ft_strdup(grp->gr_name);
  new_cache->next = group_cache;
  group_cache = new_cache;

  return new_cache->group_name;
}

FileInfo **input_list_to_table(Input input) {
  FileInfo **tab = malloc(sizeof(FileInfo *) * (input.size + 1));
  List *list = input.list;
  int i = 0;

  while (list != NULL) {
    tab[i] = list->data;

    i++;
    list = list->next;
  }

  tab[i] = NULL;

  return tab;
}
