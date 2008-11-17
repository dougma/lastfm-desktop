/***************************************************************************
 *   Copyright 2007-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/


#include <QVariant>
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QThreadStorage>
#include <vector>
#include <sqlite3.h>


QThreadStorage< std::vector< std::vector<int> >* > tl_pMatrix; // matrix

size_t string_length(const unsigned char* s)
{
    return strlen((const char*) s);
}

size_t string_length(const unsigned short* s)
{
    return wcslen(s);
}


template<typename C>
float 
normalized_levenshtein(const C* s, const C* t)
{
    size_t i; // iterates through s
    size_t j; // iterates through t
    int cost; // cost

    // Step 1
    size_t n = string_length(s);
    size_t m = string_length(t);
    if (!n || !m) {
        return 0.0;
    }
    size_t max_length = std::max(n, m);

    if (!tl_pMatrix.hasLocalData()) {
        tl_pMatrix.setLocalData(new std::vector< std::vector<int> >);
    }
    static std::vector< std::vector< int> >& d = *tl_pMatrix.localData();

    d.resize(n + 1);
    for (std::vector< std::vector< int> >::iterator it = d.begin(); it != d.end(); ++it)
        it->resize(m + 1);

    // Step 2
    for (i = 0; i <= n; i++) {
        d[i][0] = i;
    }
    for (j = 0; j <= m; j++) {
        d[0][j] = j;
    }

    // Step 3
    for (i = 1; i <= n; i++) {
        // Step 4
        for (j = 1; j <= m; j++) {
            // Step 5
            cost = (s[i - 1] == t[j - 1]) ? 0 : 1; // 0.0F : 1.0F;

            // Step 6
            d[i][j] = std::min(d[i - 1][j] + 1, std::min(d[i][j - 1] + 1, d[i - 1][j - 1] + cost));
        }
    }

    // Step 7
    int levenshtein = d[n][m];

    // normalized
    return 1.0 - (levenshtein / (float) max_length);
}

// utf8 string
static
void
user_levenshtein(sqlite3_context* ctx, int argc, sqlite3_value** argv)
{
    if (argc == 2) {
        const unsigned char *a = sqlite3_value_text(argv[0]);
        const unsigned char *b = sqlite3_value_text(argv[1]);

        if (a && b) {
            float r = normalized_levenshtein(a, b);
            sqlite3_result_double(ctx, r);
        }
    }
    // else do we need to raise an error?
}

// utf16 string in native byte order
static
void
user_levenshtein16(sqlite3_context* ctx, int argc, sqlite3_value** argv)
{
    if (argc == 2) {
        const unsigned short *a = static_cast<const unsigned short*>(sqlite3_value_text16(argv[0]));
        const unsigned short *b = static_cast<const unsigned short*>(sqlite3_value_text16(argv[1]));

        if (a && b) {
            float r = normalized_levenshtein(a, b);
            sqlite3_result_double(ctx, r);
        }
    }
    // else do we need to raise an error?
}


int 
addUserFuncs_sqlite(sqlite3* handle)
{
    int r1 = sqlite3_create_function(
        handle,
        "levenshtein",
        2,                      // parameters
        SQLITE_UTF8,            // preferred encoding
        NULL,                   // user data
        user_levenshtein,       // xFunc
        NULL,                   // xStep (for aggregate func)
        NULL);                  // xFinal (for aggregate func)

    int r2 = sqlite3_create_function(
        handle,
        "levenshtein",
        2,                      // parameters
        SQLITE_UTF16,           // preferred encoding
        NULL,                   // user data
        user_levenshtein16,     // xFunc
        NULL,                   // xStep (for aggregate func)
        NULL);                  // xFinal (for aggregate func)

    return r1 | r2;
}


int 
addUserFuncs(QSqlDatabase db)
{
    QString driver = db.driverName();
    QVariant v = db.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0) {
        sqlite3* handle = *static_cast<sqlite3 **>(v.data());
        if (handle) {
            return addUserFuncs_sqlite(handle);
        }
    }
    return -1;
}
