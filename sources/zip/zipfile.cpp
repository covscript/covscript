#include "zipfile.hpp"

#include "zip.hpp"
#include <cstdlib>
#include <cstring>
#include <zlib.h>

#define DEF_MEM_LEVEL 8                // normally in zutil.h?

enum {
    STORED = 0,
    DEFLATED = 8
};

zipfile_t
init_zipfile(const void *data, size_t size) {
    int err;

    auto file = static_cast<CZipFile *>(malloc(sizeof(CZipFile)));
    if (file == nullptr) return nullptr;
    memset(file, 0, sizeof(CZipFile));
    file->buf = static_cast<const unsigned char *>(data);
    file->bufsize = size;

    err = read_central_dir(file);
    if (err != 0) goto fail;

    return file;
    fail:
    free(file);
    return nullptr;
}

void
release_zipfile(zipfile_t f) {
    auto *file = (CZipFile *) f;
    CZipEntry *entry = file->entries;
    while (entry) {
        CZipEntry *next = entry->next;
        free(entry);
        entry = next;
    }
    free(file);
}

zipentry_t
lookup_zipentry(zipfile_t f, const char *entryName) {
    CZipFile *file = (CZipFile *) f;
    CZipEntry *entry = file->entries;
    while (entry) {
        if (0 == memcmp(entryName, entry->fileName, entry->fileNameLength)) {
            return entry;
        }
        entry = entry->next;
    }
    return nullptr;
}

size_t
get_zipentry_size(zipentry_t entry) {
    return ((CZipEntry *) entry)->uncompressedSize;
}

char *
get_zipentry_name(zipentry_t entry) {
    CZipEntry *e = (CZipEntry *) entry;
    auto l = static_cast<int>(e->fileNameLength);
    auto *s = static_cast<char *>(malloc(l + 1));
    memcpy(s, e->fileName, static_cast<size_t>(l));
    s[l] = '\0';
    return s;
}

static int
uninflate(unsigned char *out, int unlen, const unsigned char *in, int clen) {
    z_stream zstream{};
    int err = 0;
    int zerr;

    memset(&zstream, 0, sizeof(zstream));
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zstream.next_in = static_cast<Bytef *>((void *) in);
    zstream.avail_in = static_cast<uInt>(clen);
    zstream.next_out = out;
    zstream.avail_out = static_cast<uInt>(unlen);
    zstream.data_type = Z_UNKNOWN;

    // Use the undocumented "negative window bits" feature to tell zlib
    // that there's no zlib header waiting for it.
    zerr = inflateInit2(&zstream, -MAX_WBITS);
    if (zerr != Z_OK) {
        return -1;
    }

    // uncompress the data
    zerr = inflate(&zstream, Z_FINISH);
    if (zerr != Z_STREAM_END) {
        fprintf(stderr, "zerr=%d Z_STREAM_END=%d total_out=%lu\n", zerr, Z_STREAM_END,
                zstream.total_out);
        err = -1;
    }

    inflateEnd(&zstream);
    return err;
}

int
decompress_zipentry(zipentry_t e, void *buf, int bufsize) {
    CZipEntry *entry = (CZipEntry *) e;
    switch (entry->compressionMethod) {
        case STORED:
            memcpy(buf, entry->data, entry->uncompressedSize);
            return 0;
        case DEFLATED:
            return uninflate(static_cast<unsigned char *>(buf), bufsize, entry->data, entry->compressedSize);
        default:
            return -1;
    }
}

zipentry_t
iterate_zipfile(zipfile_t file, void **cookie) {
    CZipEntry *entry = (CZipEntry *) *cookie;
    if (entry == nullptr) {
        CZipFile *zip = (CZipFile *) file;
        *cookie = zip->entries;
        return *cookie;
    } else {
        entry = entry->next;
        *cookie = entry;
        return entry;
    }
}
