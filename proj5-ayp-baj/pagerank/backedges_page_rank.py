from simple_page_rank import SimplePageRank

"""
This class implements the pagerank algorithm with
backwards edges as described in the second part of 
the project.
"""
class BackedgesPageRank(SimplePageRank):

    """
    The implementation of __init__ and compute_pagerank should 
    still be the same as SimplePageRank.
    You are free to override them if you so desire, but the signatures
    must remain the same.
    """

    """
    This time you will be responsible for implementing the initialization
    as well. 
    Think about what additional information your data structure needs 
    compared to the old case to compute weight transfers from pressing
    the 'back' button.
    """
    @staticmethod
    def initialize_nodes(input_rdd):
        # YOUR CODE HERE
        # The pattern that this solution uses is to keep track of 
        # (node, (weight, targets, old_weight)) for each iteration.
        # When calculating the score for the next iteration, you
        # know that 10% of the score you sent out from the previous
        # iteration will get sent back.
        # takes in a line and emits edges in the graph corresponding to that line
        def emit_edges(line):
            # ignore blank lines and comments
            if len(line) == 0 or line[0] == "#":
                return []
            # get the source and target labels
            source, target = tuple(map(int, line.split()))
            # emit the edge
            edge = (source, frozenset([target]))
            # also emit "empty" edges to catch nodes that do not have any
            # other node leading into them, but we still want in our list of nodes
            self_source = (source, frozenset())
            self_target = (target, frozenset())
            return [edge, self_source, self_target]

        # collects all outgoing target nodes for a given source node
        def reduce_edges(e1, e2):
            return e1 | e2 

        # sets the weight of every node to 0, and formats the output to the 
        # specified format of (source (weight, targets))
        def initialize_weights((source, targets)):
            #initialize the previous weights
            prevWeights = {}
            prevWeights[source] = 1.0
            return (source, (1.0, targets, prevWeights))

        nodes = input_rdd\
                .flatMap(emit_edges)\
                .reduceByKey(reduce_edges)\
                .map(initialize_weights)
        return nodes

    """
    You will also implement update_weights and format_output from scratch.
    You may find the distribute and collect pattern from SimplePageRank
    to be suitable, but you are free to do whatever you want as long
    as it results in the correct output.
    """
    @staticmethod
    def update_weights(nodes, num_nodes):
        """
        Mapper phase.
        Distributes pagerank scores for a given node to each of its targets,
        as specified by the update algorithm.
        Some important things to consider:
        We can't just emit (target, weight) values to the reduce phase, 
        because then the reduce phase will lose information on the outgoing edges
        for the nodes. We have to emit the (node, targets) pairs too so that 
        the edges can be remembered for the next iteration.
        Think about the best output format for the mapper so the reducer can
        get both types of information.
        You are allowed to change the signature if you desire to.
        """
        def distribute_weights((node, (weight, targets, prevWeights))):
            # YOUR CODE HERE
            # 0.05, 0.85, 0.10

            s = len(targets)
            l = []

            #distribute based on edges: 85%
            for target in targets:
                l.append((target, ((weight * 0.85)/s, node, None)))

            #distribute if no edges: 85% to all nodes at random
            if not s:
                for target in range(num_nodes):
                    if not target == node:
                        l.append((target, ((weight * 0.85)/(num_nodes - 1), node, None)))

            #staying, 5%
            l.append((node, ((weight * 0.05), node, targets)))

            #back button
            for k in prevWeights:
                l.append((k, (prevWeights[k]*0.1, node, None)))

            return l

        """
        Reducer phase.
        We are given a node as a key and a list of all the values emitted by the mappers
        corresponding to that key.
        There should be two types of values:
        Pagerank scores, which represent how much score an incoming node is giving to us,
        and edge data, which we need to collect and store for the next iteration.
        The output of this phase should be in the same format as the input to the mapper.
        You are allowed to change the signature if you desire to.
        """
        def collect_weights((node, values)):
            # YOUR CODE HERE
            weight = 0
            targets = []
            prevWeights = {}

            for value in values:
                weight += value[0]
                if value[1] not in prevWeights:
                    prevWeights[value[1]] = 0
                prevWeights[value[1]] += value[0]
                if value[2]:
                    targets = value[2]

            return (node, (weight, targets, prevWeights))

        return nodes\
                .flatMap(distribute_weights)\
                .groupByKey()\
                .map(collect_weights)
                     
    @staticmethod
    def format_output(nodes): 
        return nodes\
                .map(lambda (node, (weight, targets, prevWeights)): (weight, node))\
                .sortByKey(ascending = False)\
                .map(lambda (weight, node): (node, weight))

