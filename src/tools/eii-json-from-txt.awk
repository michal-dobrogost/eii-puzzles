#!/usr/bin/env -S awk -f
{
  if (NR == 1) {
    printf("{\n")
    printf("  \"name\": \"%s\",\n", FILENAME)
    printf("  \"gen\": null,\n")
    printf("  \"rows\": %s,\n", $1)
    printf("  \"cols\": %s,\n", $2)
    printf("  \"tiles\": [\n")
  } else {
    if (NR >= 3) {
      printf(",\n")
    }
    printf("    [%s, %s, %s, %s]", $1, $2, $3, $4)
  }
}
END {
  printf("\n")
  printf("  ],\n")
  printf("  \"placed\": []\n")
  printf("}\n")
}
