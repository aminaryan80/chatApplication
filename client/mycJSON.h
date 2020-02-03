
/* The cJSON structure: */
typedef struct cJSON
{
    char key[20];

    char valuestring[200];

    struct myJSON *object[100];
    int objectSize;

    struct myJSON *arrayObject[100];
    int arraySize;
} cJSON;

cJSON * cJSON_Parse(const char *value);
cJSON * cJSON_GetObjectItem(const cJSON * const object, const char * const string);
cJSON * cJSON_CreateObject();
cJSON * cJSON_CreateString(const char * const string);
cJSON * cJSON_AddStringToObject(cJSON * object, const char * const key, const char * const valuestring);
void cJSON_AddItemToArray(cJSON * array, const char * const item);
cJSON * cJSON_AddArrayToObject(cJSON * object, const char * const key);
int cJSON_GetArraySize(const cJSON * const array);
cJSON * cJSON_GetArrayItem(const cJSON * const array, int index);
char * cJSON_Print(const cJSON * const object);
void cJSON_Delete(cJSON *item);
