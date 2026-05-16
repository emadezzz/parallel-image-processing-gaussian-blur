#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int w, h;
    unsigned char *pix;
} Image;

static int read_ppm(const char *path, Image *im){
    FILE *f = fopen(path, "rb");
    if(!f) return 1;
    char m[3];
    if(fscanf(f, "%2s", m) != 1 || strcmp(m, "P6") != 0){ fclose(f); return 2; }
    int c = getc(f);
    while(c == '#'){ while(c != '\n' && c != EOF) c = getc(f); c = getc(f); }
    ungetc(c, f);
    int maxv;
    if(fscanf(f, "%d %d %d", &im->w, &im->h, &maxv) != 3){ fclose(f); return 3; }
    if(maxv != 255){ fclose(f); return 4; }
    fgetc(f);
    size_t n = (size_t)im->w * im->h * 3;
    im->pix = malloc(n);
    if(!im->pix){ fclose(f); return 5; }
    if(fread(im->pix, 1, n, f) != n){ free(im->pix); fclose(f); return 6; }
    fclose(f);
    return 0;
}

static int write_ppm(const char *path, const Image *im){
    FILE *f = fopen(path, "wb");
    if(!f) return 1;
    fprintf(f, "P6\n%d %d\n255\n", im->w, im->h);
    size_t n = (size_t)im->w * im->h * 3;
    fwrite(im->pix, 1, n, f);
    fclose(f);
    return 0;
}

static inline unsigned char sat(int v){
    if(v < 0) return 0;
    if(v > 255) return 255;
    return (unsigned char)v;
}

int main(){
    Image in = {0}, out = {0};
    if(read_ppm("data/input.ppm", &in)) return 1;
    out.w = in.w;
    out.h = in.h;
    out.pix = malloc((size_t)in.w * in.h * 3);
    int w = in.w, h = in.h;
    unsigned char *src = in.pix;
    unsigned char *dst = out.pix;
    int radius = 50;
    clock_t start = clock();
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            int r = 0, g = 0, b = 0, count = 0;
            for(int dy = -radius; dy <= radius; dy++){
                for(int dx = -radius; dx <= radius; dx++){
                    int yy = y + dy;
                    int xx = x + dx;
                    if(yy >= 0 && yy < h && xx >= 0 && xx < w){
                        const unsigned char *p = &src[(yy*w + xx)*3];
                        r += p[0];
                        g += p[1];
                        b += p[2];
                        count++;
                    }
                }
            }
            dst[(y*w + x)*3 + 0] = sat(r / count);
            dst[(y*w + x)*3 + 1] = sat(g / count);
            dst[(y*w + x)*3 + 2] = sat(b / count);
        }
    }
    clock_t end = clock();
    write_ppm("data/output_seq.ppm", &out);

    
    system("convert data/output_seq.ppm data/output_seq.jpg");

    double real_s = (double)(end - start) / CLOCKS_PER_SEC;
    FILE *csv = fopen("results.csv", "a");
    if(csv){
        fprintf(csv, "seq,0,%.3f\n", real_s);
        fclose(csv);
    }
    free(in.pix);
    free(out.pix);
system("awk -F, '{if ($1 == \"seq\") { seq_time += $3; seq_count++ } else if ($1 == \"mt\") { mt_time[$2] += $3; mt_count[$2]++ }} END { if (seq_count) printf \"seq_avg,0,%.3f\\n\", seq_time / seq_count; for (t in mt_time) if (mt_count[t]) printf \"mt_avg,%d,%.3f\\n\", t, mt_time[t] / mt_count[t] }' results.csv > summary.csv");
    return 0;
}