#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "mycJSON.h"

cJSON * cJSON_ParseObject(const char *value)
{
    cJSON *tmp = calloc(1, sizeof(cJSON));
    int count = 1;
    for (int i = 0; value[i]; i++)
    {
        if(value[i]==',')
        {
            count++;
        }

    }
    tmp->objectSize = count;
    char *string = calloc(1, 5120);
    strcpy(string, value);
    string++;
    char *items;
    items = calloc(1, 1000);
    items = strtok(string, ",");
    //printf("object : %s\n", items);
    tmp->object[0] = cJSON_Parse(items);
    int i = 0;
    while((items = strtok(NULL, ",")) != NULL)
    {
        //printf("object : %s\n", items);
        tmp->object[++i] = cJSON_Parse(items);
    }
    free(items);
    free(string);
    return tmp;
}

cJSON * cJSON_ParseArray(const char *value)
{
    if(strchr(value + 1, '{') || strstr(value, "[]"))   // refresh
    {
        cJSON *tmp = calloc(1, sizeof(cJSON));
        if(value[1] != 'm')
            strcpy(tmp->key, "content");
        else
            strcpy(tmp->key, "messages");
        int count = 0;
        for (int i = 0; value[i]; i++)
        {
            if(value[i]=='{')
            {
                count++;
            }
        }
        tmp->arraySize = count;
        if(!count)
        {
            return tmp;
        }
        char *string = calloc(1, 5120);
        strcpy(string, strchr(value, ':'));
        string += 2;
        char *items;
        items = calloc(1, 1000);
        items = strtok(string, "}");
        //printf("array : %s\n", string);
        tmp->arrayObject[0] = cJSON_Parse(string);
        int i = 0;
        strcpy(string, value);
        items = strtok(string, "}");
        while((items = strtok(NULL, "}")) != ']')
        {
            items++;
            if(i + 1 == count)
            {
                break;
            }
            if(++i != count)
                tmp->arrayObject[i] = cJSON_Parse(items);
            strcpy(string, value);
            items = strtok(string, "}");
            for(int j = 0; j < i; j++)
                items = strtok(NULL, "}");
        }
        return tmp;
    }
    else     // channel members
    {
        cJSON *tmp = calloc(1, sizeof(cJSON));
        strcpy(tmp->key, "content");
        int count = 1;
        for (int i = 0; value[i]; i++)
        {
            if(value[i]==',')
            {
                count++;
            }

        }
        tmp->arraySize = count;
        char *string = calloc(1, 5120);
        strcpy(string, value);
        string++;
        char *items;
        items = calloc(1, 1000);
        items = strtok(string, ",");
        //printf("array : %s\n", items);
        tmp->arrayObject[0] = cJSON_Parse(items);
        int i = 0;
        while((items = strtok(NULL, ",")) != NULL)
        {
            //printf("array : %s\n", items);
            tmp->arrayObject[++i] = cJSON_Parse(items);
        }
        free(items);
        free(string);
        return tmp;
    }
}

cJSON * cJSON_Parse(const char *value)
{
    if(strncmp(value, "{", 1) == 0)
    {
        if (strstr(value, "["))
        {
            cJSON * tmp = calloc(1, sizeof(cJSON));
            tmp->objectSize = 2;
            char *string = calloc(1,5120);
            char *n = strchr(value, ',');
            strncpy(string, value + 1, n - (value + 1));
            tmp->object[0] = cJSON_Parse(string);
            tmp->object[1] = cJSON_ParseArray(strchr(value, ',') + 1);
            return tmp;
        }
        return cJSON_ParseObject(value);
    }
    cJSON * tmp = calloc(1, sizeof(cJSON));
    char *string = calloc(1, 5120);
    strcpy(string, value);
    char *f = strchr(string, '\"');
    char *l = strchr(++f, '\"');
    char *f2 = strchr(string, ':');
    if(f2)
    {
        strncpy(tmp->key, f, l - f);
        f = strchr(f2, '\"');
        l = strchr(++f, '\"');
        strncpy(tmp->valuestring, f, l - f);
    }
    else
    {
        strncpy(tmp->valuestring, f, l - f);
    }
    //printf("item : %s\n", tmp->valuestring);
    free(string);
    return tmp;
}

cJSON * cJSON_GetObjectItem(const cJSON * const object, const char * const string)
{
    for(int i = 0; i < object->objectSize; i++)
    {
        cJSON *subObject = object->object[i];
        if(!strcmp(subObject->key, string))
        {
            return subObject;
        }
    }
    return NULL;
}

cJSON * cJSON_CreateObject()
{
    cJSON * tmp = calloc(1, sizeof(cJSON));
    return tmp;
}

cJSON * cJSON_CreateString(const char * const string)
{
    cJSON * tmp = calloc(1, sizeof(cJSON));
    strcpy(tmp->valuestring, string);
    return tmp;
}

cJSON * cJSON_AddStringToObject(cJSON * object, const char * const key, const char * const valuestring)
{
    cJSON * tmp = calloc(1, sizeof(cJSON));
    strcpy(tmp->key, key);
    strcpy(tmp->valuestring, valuestring);
    int index = object->objectSize;
    object->object[index] = tmp;
    object->objectSize += 1;
    return tmp;
}

void cJSON_AddItemToArray(cJSON * array, const char * const item)
{
    int index = array->arraySize;
    array->arrayObject[index] = item;
    array->arraySize += 1;
}

cJSON * cJSON_AddArrayToObject(cJSON * object, const char * const key)
{
    cJSON * tmp = calloc(1, sizeof(cJSON));
    strcpy(tmp->key, key);
    int index = object->objectSize;
    object->object[index] = tmp;
    object->objectSize += 1;
    return tmp;
}

int cJSON_GetArraySize(const cJSON * const array)
{
    return array->arraySize;
}

cJSON * cJSON_GetArrayItem(const cJSON * const array, int index)
{
    return array->arrayObject[index];
}

char * cJSON_Print(const cJSON * const object)
{
    char * result = calloc(1, 5120);
    strcat(result, "{");
    if(object->object[0] != NULL)
    {
        for(int i = 0; i < object->objectSize; i++)
        {
            cJSON *tmp = object->object[i];
            if(strcmp(tmp->valuestring, ""))
            {
                if(!strcmp(tmp->valuestring, "-1"))
                    strcpy(tmp->valuestring, "");
                strcat(result, "\"");
                strcat(result, tmp->key);
                strcat(result, "\":\"");
                strcat(result, tmp->valuestring);
                strcat(result, "\"");
            }
            else if (tmp->arrayObject[0] != NULL)
            {
                strcat(result, "\"");
                strcat(result, tmp->key);
                strcat(result, "\":[");
                for(int j = 0; j < tmp->arraySize; j++)
                {
                    cJSON *tmp2 = tmp->arrayObject[j];
                    if(strcmp(tmp2->valuestring, ""))
                    {
                        strcat(result, "\"");
                        strcat(result, tmp2->valuestring);
                        strcat(result, "\"");
                        strcat(result, ",");
                    }
                    else if (tmp2->object[0] != NULL)
                    {
                        cJSON *sender = tmp2->object[0];
                        cJSON *content = tmp2->object[1];
                        strcat(result, "{\"sender\":\"");
                        strcat(result, sender->valuestring);
                        strcat(result, "\",\"content\":\"");
                        strcat(result, content->valuestring);
                        strcat(result, "\"}");
                        strcat(result, ",");
                    }
                }
                result[strlen(result) - 1] = ']';
            }
            strcat(result, ",");
        }
        result[strlen(result) - 1] = '}';
        return result;
    }
}

void cJSON_Delete(cJSON *item)
{
    int i = 0;
    while(item->arrayObject[i] != NULL)
    {
        cJSON_Delete(item->arrayObject[i++]);
    }
    i = 0;
    while(item->object[i] != NULL)
    {
        cJSON_Delete(item->object[i++]);
    }
    strcpy(item->key, "");
    strcpy(item->valuestring, "");
    item->objectSize = 0;
    item->arraySize = 0;
    realloc(item, 0);
}

