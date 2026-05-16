#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int w, h;
    unsigned char *pix;
} Image;

typedef struct {
    const Image *in;
    Image *out;
    int y0, y1;
    int radius;
} Task;

static int read_ppm(const char *path, Image *im){
    FILE *f = fopen(path, "rb");
    if (!f) return 1;
    char m[3];
    if (fscanf(f, "%2s", m) != 1 || strcmp(m, "P6") != 0){ fclose(f); return 2; }
    int c = getc(f);
    while (c == '#'){ while (c != '\n' && c != EOF) c = getc(f); c = getc(f); }
    ungetc(c, f);
    int maxv;
    if (fscanf(f, "%d %d %d", &im->w, &im->h, &maxv) != 3){ fclose(f); return 3; }
    if (maxv != 255){ fclose(f); return 4; }
    fgetc(f);
    size_t n = (size_t)im->w * im->h * 3;
    im->pix = malloc(n);
    if (!im->pix){ fclose(f); return 5; }
    if (fread(im->pix, 1, n, f) != n){ free(im->pix); fclose(f); return 6; }
    fclose(f);
    return 0;
}

static int write_ppm(const char *path, const Image *im){
    FILE *f = fopen(path, "wb");
    if (!f) return 1;
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

static void *worker(void *arg){
    Task *t = (Task*)arg;
    int w = t->in->w;
    int h = t->in->h;
    const unsigned char *src = t->in->pix;
    unsigned char *dst = t->out->pix;
    int R = t->radius;
    for(int y = t->y0; y < t->y1; y++){
        for(int x = 0; x < w; x++){
            int r = 0, g = 0, b = 0, count = 0;
            for(int dy = -R; dy <= R; dy++){
                for(int dx = -R; dx <= R; dx++){
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
            dst[(y*w+x)*3 + 0] = sat(r / count);
            dst[(y*w+x)*3 + 1] = sat(g / count);
            dst[(y*w+x)*3 + 2] = sat(b / count);
        }
    }
    return NULL;
}

int main(int argc, char **argv){
    if(argc < 3) return 1;
    const char *inpath = argv[1];
    int T = atoi(argv[2]);
    if(T < 1) T = 1;
    Image in = {0}, out = {0};
    if(read_ppm(inpath, &in)) return 2;
    out.w = in.w;
    out.h = in.h;
    out.pix = malloc((size_t)in.w * in.h * 3);
    pthread_t *ths = malloc(sizeof(pthread_t) * T);
    Task *tasks = malloc(sizeof(Task) * T);
    int rows = in.h;
    int chunk = (rows + T - 1) / T;
    int radius = 50;
    clock_t start = clock();
    for(int i = 0; i < T; i++){
        tasks[i].in = &in;
        tasks[i].out = &out;
        tasks[i].y0 = i * chunk;
        tasks[i].y1 = (i+1) * chunk;
        if(tasks[i].y1 > rows) tasks[i].y1 = rows;
        tasks[i].radius = radius;
        pthread_create(&ths[i], NULL, worker, &tasks[i]);
    }
    for(int i = 0; i < T; i++) pthread_join(ths[i], NULL);
    clock_t end = clock();
    char outpath[256];
    snprintf(outpath, sizeof(outpath), "data/output_mt_%d.ppm", T);
    write_ppm(outpath, &out);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "convert %s data/output_mt_%d.jpg", outpath, T);
    system(cmd);

    double real_s = (double)(end - start) / CLOCKS_PER_SEC;
    FILE *csv = fopen("results.csv", "a");
    if(csv){
        fprintf(csv, "mt,%d,%.3f\n", T, real_s);
        fclose(csv);
    }
    free(ths);
    free(tasks);
    free(in.pix);
    free(out.pix);
system("awk -F, '{if ($1 == \"seq\") { seq_time += $3; seq_count++ } else if ($1 == \"mt\") { mt_time[$2] += $3; mt_count[$2]++ }} END { if (seq_count) printf \"seq_avg,0,%.3f\\n\", seq_time / seq_count; for (t in mt_time) if (mt_count[t]) printf \"mt_avg,%d,%.3f\\n\", t, mt_time[t] / mt_count[t] }' results.csv > summary.csv");
    return 0;
}