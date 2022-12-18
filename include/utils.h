#ifndef UTILS_H
#define UTILS_H

const int MAX_NAME_LENGTH = 50;

struct graph_node_atr_t {
        const char *shape = "rectangle";
        const char *style = "rounded";
        const char *fixedsize = "false";
        const char *fillcolor = "#00b899";
        int height = 3;
        int width = 2;
        int fontsize = 30;
        int penwidth = 5;
};

struct edge_t {
        int penwidth = 5;
        const char *color = "black";
};

struct digraph_t {
        int dpi = 300;
        const char *splines = "ortho";
};

#endif /*UTILS_H*/
