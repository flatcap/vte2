#ifdef TERMCAP_MAIN
#include <stdio.h>

int
main (int argc, char **argv)
{
  VteTermcap *tc;
  char *str;
  gssize len;
  int i;

  if (argc < 4)
  {
    g_printerr("vtetc /path/to/termcap termname attrs...\n"
                     "  where attrs are\n"
                     "    :xx for boolean\n"
                     "    =xx for string\n"
                     "    +xx for string displayed in hex\n"
                     "    #xx for numeric\n");
    return 1;
  }

  tc = _vte_termcap_new (argv[1]);

  if (tc == NULL)
  {
    perror ("open");
    return 1;
  }

  for (i = 3; i < argc; i++)
  {
    printf ("%s -> ", argv[i]);

    switch (argv[i][0])
    {
      case ':':
        printf ("%s\n", _vte_termcap_find_boolean (tc, argv[2], argv[i] + 1)?
                        "true" : "false");
        break;

      case '=':
      case '+':
        str = _vte_termcap_find_string_length (tc, argv[2], argv[i] + 1, &len);

        if (argv[i][0] == '=')
          printf ("'%s' (%d)\n", str, (int)len);
        else
        {
          int i;

          for (i = 0; str[i]; i++)
            printf ("%02x", str[i]);
          printf (" (%d) \n", (int)len);
        }
        g_free (str);
        break;

      case '#':
        printf ("%ld\n", _vte_termcap_find_numeric (tc, argv[2], argv[i] + 1));
        break;

      default:
        g_printerr("unrecognised type '%c'\n", argv[i][0]);
    }
  }

  _vte_termcap_free(tc);

  return 0;
}
#endif
