/*
** Copyright (C) 2003-2006 Nullsoft, Inc.
**
** This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held 
** liable for any damages arising from the use of this software. 
**
** Permission is granted to anyone to use this software for any purpose, including commercial applications, and to 
** alter it and redistribute it freely, subject to the following restrictions:
**
**   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. 
**      If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
**
**   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
**
**   3. This notice may not be removed or altered from any source distribution.
**
*/

#include "StdAfx.h"
#include "ml.h"

void freeRecord(itemRecord *p) {
  free(p->title);
  free(p->artist);
  free(p->comment);
  free(p->album);
  free(p->genre);
  free(p->filename);
  if (p->extended_info)
  {
    int x=0;
    for (x = 0; p->extended_info[x]; x ++)
      free(p->extended_info[x]);
    free(p->extended_info);
  }
  memset(p,0,sizeof(itemRecord));
}

void freeRecordList(itemRecordList *obj)
{
  emptyRecordList(obj);
  free(obj->Items);
  obj->Items=0;
  obj->Alloc=obj->Size=0;
}

void emptyRecordList(itemRecordList *obj)
{
  itemRecord *p=obj->Items;
  while (obj->Size-->0)
  {
    freeRecord(p);
    p++;
  }
  obj->Size=0;
}

void allocRecordList(itemRecordList *obj, int newsize, int granularity)
{
  if (newsize < obj->Alloc || newsize < obj->Size) return;

  obj->Alloc=newsize+granularity;
  obj->Items=(itemRecord*)realloc(obj->Items,sizeof(itemRecord)*obj->Alloc);
  if (!obj->Items) obj->Alloc=0;
}

void copyRecord(itemRecord *out, itemRecord *in)
{
  int y;
#define COPYSTR(FOO) out->FOO = in->FOO ? strdup(in->FOO) : 0;
  COPYSTR(filename)
  COPYSTR(title)
  COPYSTR(album)
  COPYSTR(artist)
  COPYSTR(comment)
  COPYSTR(genre)
  out->year=in->year;
  out->track=in->track;
  out->length=in->length;
#undef COPYSTR
  out->extended_info=0;

  if (in->extended_info) for (y = 0; in->extended_info[y]; y ++)
  {
    char *p=in->extended_info[y];
    if (*p) setRecordExtendedItem(out,p,p+strlen(p)+1);
  }
}

void copyRecordList(itemRecordList *out, itemRecordList *in)
{
  int x;
  allocRecordList(out,out->Size+in->Size,0);
  if (!out->Items) return;
  for (x = 0; x < in->Size; x ++)
  {
    copyRecord(&out->Items[out->Size++],&in->Items[x]);
  }
}

char *getRecordExtendedItem(const itemRecord *item, const char *name)
{
  int x=0;
  if (item->extended_info) for (x = 0; item->extended_info[x]; x ++)
  {
    if (!stricmp(item->extended_info[x],name))
      return item->extended_info[x]+strlen(name)+1;
  }
  return NULL;
}

void setRecordExtendedItem(itemRecord *item, const char *name, char *value)
{
  int x=0;
  if (item->extended_info) for (x = 0; item->extended_info[x]; x ++)
  {
    if (!stricmp(item->extended_info[x],name))
    {
      if (strlen(value)>strlen(item->extended_info[x]+strlen(name)+1))
      {
        free(item->extended_info[x]);
        item->extended_info[x]=(char*)malloc(strlen(name)+strlen(value)+2);
      }
      strcpy(item->extended_info[x],name);
      strcpy(item->extended_info[x]+strlen(name)+1,value);
      return;
    }
  }
  // x=number of valid items.
  item->extended_info=(char**)realloc(item->extended_info,sizeof(char*) * (x+2));
  if (item->extended_info)
  {
    item->extended_info[x]=(char*)malloc(strlen(name)+strlen(value)+2);
    strcpy(item->extended_info[x],name);
    strcpy(item->extended_info[x]+strlen(name)+1,value);

    item->extended_info[x+1]=0;
  }
}

/*
---------------------------------- 
wide version starts here
---------------------------------- 
*/
 
 void freeRecord(itemRecordW *p) {
  free(p->title);
  free(p->artist);
  free(p->comment);
  free(p->album);
  free(p->genre);
  free(p->filename);
	free(p->albumartist); 
	free(p->replaygain_album_gain); 
	free(p->replaygain_track_gain); 
	free(p->publisher);
	free(p->composer);
  if (p->extended_info)
  {
    int x=0;
    for (x = 0; p->extended_info[x]; x ++)
      free(p->extended_info[x]);
    free(p->extended_info);
  }
  memset(p,0,sizeof(itemRecordW));
}

void freeRecordList(itemRecordListW *obj)
{
  emptyRecordList(obj);
  free(obj->Items);
  obj->Items=0;
  obj->Alloc=obj->Size=0;
}

void emptyRecordList(itemRecordListW *obj)
{
  itemRecordW *p=obj->Items;
  while (obj->Size-->0)
  {
    freeRecord(p);
    p++;
  }
  obj->Size=0;
}

void allocRecordList(itemRecordListW *obj, int newsize, int granularity)
{
  if (newsize < obj->Alloc || newsize < obj->Size) return;

  obj->Alloc=newsize+granularity;
  obj->Items=(itemRecordW*)realloc(obj->Items,sizeof(itemRecordW)*obj->Alloc);
  if (!obj->Items) obj->Alloc=0;
}

void copyRecord(itemRecordW *out, itemRecordW *in)
{
  int y;
#define COPYSTR(FOO) out->FOO = in->FOO ? wcsdup(in->FOO) : 0;
#define COPY(FOO) out->FOO = in->FOO;
  COPYSTR(filename)
  COPYSTR(title)
  COPYSTR(album)
  COPYSTR(artist)
  COPYSTR(comment)
  COPYSTR(genre)
	COPYSTR(albumartist); 
	COPYSTR(replaygain_album_gain); 
	COPYSTR(replaygain_track_gain); 
	COPYSTR(publisher);
	COPYSTR(composer);
	COPY(year);
	COPY(track);
	COPY(tracks);
	COPY(length);
	COPY(rating);
  COPY(playcount); 
	COPY(lastplay); 
	COPY(lastupd); 
	COPY(filetime);
	COPY(filesize);
	COPY(bitrate); 
	COPY(type); 
	COPY(disc); 
	COPY(bpm);
	COPY(discs);  
#undef COPYSTR
  out->extended_info=0;

  if (in->extended_info) for (y = 0; in->extended_info[y]; y ++)
  {
    wchar_t *p=in->extended_info[y];
    if (*p) setRecordExtendedItem(out,p,p+wcslen(p)+1);
  }
}

void copyRecordList(itemRecordListW *out, itemRecordListW *in)
{
  int x;
  allocRecordList(out,out->Size+in->Size,0);
  if (!out->Items) return;
  for (x = 0; x < in->Size; x ++)
  {
    copyRecord(&out->Items[out->Size++],&in->Items[x]);
  }
}

wchar_t *getRecordExtendedItem(const itemRecordW *item, const wchar_t *name)
{
  int x=0;
  if (item->extended_info) for (x = 0; item->extended_info[x]; x ++)
  {
    if (!wcsicmp(item->extended_info[x],name))
      return item->extended_info[x]+wcslen(name)+1;
  }
  return NULL;
}

void setRecordExtendedItem(itemRecordW *item, const wchar_t *name, wchar_t *value)
{
  int x=0;
  if (item->extended_info) for (x = 0; item->extended_info[x]; x ++)
  {
    if (!wcsicmp(item->extended_info[x],name))
    {
      if (wcslen(value)>wcslen(item->extended_info[x]+wcslen(name)+1))
      {
        free(item->extended_info[x]);
        item->extended_info[x]=(wchar_t*)malloc((wcslen(name)+wcslen(value)+2)*sizeof(wchar_t));
      }
      wcscpy(item->extended_info[x],name);
      wcscpy(item->extended_info[x]+wcslen(name)+1,value);
      return;
    }
  }
  // x=number of valid items.
  item->extended_info=(wchar_t**)realloc(item->extended_info,sizeof(wchar_t*) * (x+2));
  if (item->extended_info)
  {
    item->extended_info[x]=(wchar_t*)malloc((wcslen(name)+wcslen(value)+2)*sizeof(wchar_t));
    wcscpy(item->extended_info[x],name);
    wcscpy(item->extended_info[x]+wcslen(name)+1,value);

    item->extended_info[x+1]=0;
  }
}
// TODO: redo this without AutoChar
#include "AutoChar.h"
#define COPY_EXTENDED_STR(field) if (input-> ## field && input-> ## field ## [0]) setRecordExtendedItem(output, #field, AutoChar(input-> ## field));
#define COPY_EXTENDED_INT(field) if (input->##field > 0) { char temp[64]; itoa(input->##field, temp, 10); setRecordExtendedItem(output, #field, temp); }
#define COPY_EXTENDED_INT0(field) if (input->##field >= 0) { char temp[64]; itoa(input->##field, temp, 10); setRecordExtendedItem(output, #field, temp); }
void convertRecord(itemRecord *output, const itemRecordW *input)
{
	output->filename=AutoCharDup(input->filename);
  output->title=AutoCharDup(input->title);
  output->album=AutoCharDup(input->album);
  output->artist=AutoCharDup(input->artist);
  output->comment=AutoCharDup(input->comment);
  output->genre=AutoCharDup(input->genre);
  output->year=input->year;
  output->track=input->track;
  output->length=input->length;
	output->extended_info=0;	
	COPY_EXTENDED_STR(albumartist);
	COPY_EXTENDED_STR(replaygain_album_gain);
	COPY_EXTENDED_STR(replaygain_track_gain);
	COPY_EXTENDED_STR(publisher);
	COPY_EXTENDED_STR(composer);
	COPY_EXTENDED_INT(tracks);
	COPY_EXTENDED_INT(rating);
	COPY_EXTENDED_INT(playcount);
	COPY_EXTENDED_INT(lastplay);
	COPY_EXTENDED_INT(lastupd);
	COPY_EXTENDED_INT(filetime);
	COPY_EXTENDED_INT(filesize);
	COPY_EXTENDED_INT0(type);
	COPY_EXTENDED_INT(disc);
	COPY_EXTENDED_INT(discs);
	COPY_EXTENDED_INT(bpm);
  // TODO: copy input's extended fields
}