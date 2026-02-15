#include <omp.h>
#include <queue>
#include "util/graph/graph.h"
#include "util/command_parser.h"
#include "util/log/log.h"

int main(int argc, char *argv[]) {
    InputParser cmd_parser(argc, argv);
    // Load graphs.
    Graph graph;
    graph.is_edge_weighted_ = true;

    graph.load_csr(cmd_parser.get_cmd_option("-f"));

    graph.print_metadata();

    log_info("Start generate weight max...");

    auto start = std::chrono::high_resolution_clock::now();

    graph.is_edge_weight_rejection_generated_ = true;
    auto rejection_max_size = sizeof(double) * static_cast<uint64_t>(graph.num_vertices_);
    graph.edge_weight_rejection_max_ = (double*)malloc(rejection_max_size);

#pragma omp parallel for schedule(dynamic, 1000)
    for (int i = 0; i < graph.num_vertices_; ++i) {
        graph.edge_weight_rejection_max_[i] = *std::max_element(graph.edge_weight_ + graph.offset_[i], graph.edge_weight_ + graph.offset_[i + 1]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    log_info("Preprocess time: %.6lf seconds",
             std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000000.0);

    graph.is_edge_weighted_ = false;
    graph.store_csr(cmd_parser.get_cmd_option("-f"));
    log_info("Done...");

    return 0;
}
