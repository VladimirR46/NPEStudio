/* Copyright 2019 The MathWorks, Inc. */

// PriorityQueueImpl Common Priority Queue data structure for all A* variants

#ifndef PRIORITYQUEUE_INTERFACE_IMPL
#define PRIORITYQUEUE_INTERFACE_IMPL

// A C++ Program to implement Priority Queue Data structure for AStar variants
#include <set>
#include <iterator>
#include <cfloat>
#include <string.h>
#include <assert.h>
#include <vector>
namespace nav {
/**
 * @brief Wraps the Priority queue data structure
 */

/** NodeData: Structure to contain node data for each node of a priority queue.
 *  By default 'NodeData' contains min of 5 data: id, parent_id, f,g and h cost.
 *  Any additional cost will be stored in the vector 'nScores'.
 */
struct NodeData {
    int id;
    int parent;
    double fScore, gScore, hScore;
    std::vector<double> nScores;

    // Default constructor for structure 'NodeData'
    NodeData(const int& idIn = 0,
             const int& parentIn = -1,
             const double& fIn = DBL_MAX,
             const double& gIn = DBL_MAX,
             const double& hIn = DBL_MAX,
             const std::vector<double>& nScoresIn = std::vector<double>())
        : id(idIn)
        , parent(parentIn)
        , fScore(fIn)
        , gScore(gIn)
        , hScore(hIn)
        , nScores(nScoresIn) {
    }

    // Overloading operator for searching and pushing using 'id'
    bool operator<(const NodeData& rhs) const {
        return id < rhs.id;
    }
    bool operator==(const NodeData& rhs) const {
        return id == rhs.id;
    }
};

class PriorityQueueImpl {
  private:
    /// Variables

    /** 'NodeSet': List of all nodes with their data.
     *   NodeSet stores the data of all the nodes for the unique id's.
     */
    std::set<NodeData> NodeSet;
    /// PriorityList: Priority queue of all the nodes, sorted using fScore.
    std::set<std::pair<double, int> > PriorityList;

    std::vector<double> getNodeVector(std::set<NodeData>::iterator it) {
        std::vector<double> outData;
        outData.push_back(it->id);
        outData.push_back(it->parent);
        outData.push_back(static_cast<double>(it->fScore));
        outData.push_back(static_cast<double>(it->gScore));
        outData.push_back(static_cast<double>(it->hScore));
        for (size_t j = 0; j < it->nScores.size(); j++) {
            outData.push_back(static_cast<double>(it->nScores.at(j)));
        }
        return outData;
    }
    
    std::vector<double> getEmptyNodeVector() {
        std::vector<double> outData;
        for (int j = 0; j < DataDimension; j++) {
            outData.push_back(0);
        }
        return outData;
    }

  public:
    /// Constructor for PriorityQueue. Takes input for the maximum dimension of data.
    PriorityQueueImpl(int dim = 5)
        : DataDimension(dim) {
            assert(dim >= 5);
    }  
        
    /// DataDimension: Stores the size of a single node;
    const int DataDimension;

    void push(const double* newNode, int numRows = 1) {
        /**push() Function push the node in the priority queue according to the
         * 'f' cost of the node.
         * Current node structure is (id, parent_id, fScore, gScore, hScore, nScores)
         * Since the first element is id, searching using that.
         */
        for (int i = 0; i < numRows; i++) {
            // Checking if node is already present, if yes then replace that node.
            std::set<NodeData>::iterator it = findNode(static_cast<int>(newNode[i]));

            // Asserting if nodes are positive.
            assert(newNode[i] > 0);
            assert(newNode[i + numRows] > 0);

            if (it != NodeSet.end()) {
                replaceNodeData(newNode,numRows);
            } else {
                NodeData node;
                // Consolidating data
                node.id = static_cast<int>(newNode[i]);
                node.parent = static_cast<int>(newNode[i + numRows]);
                node.fScore = static_cast<double>(newNode[i + numRows * 2]);
                node.gScore = static_cast<double>(newNode[i + numRows * 3]);
                node.hScore = static_cast<double>(newNode[i + numRows * 4]);

                for (int j = 5; j < DataDimension; j++) {
                    node.nScores.push_back(static_cast<double>(newNode[i + numRows * j]));
                }
                // Inserting the node.
                NodeSet.insert(node);
                PriorityList.insert(std::make_pair(node.fScore, node.id));
            }
        }
    }

    std::vector<double> top() {
        /// top() Function returns the lowest cost node in the priority queue.
        if (isEmpty()) {
            return getEmptyNodeVector();
        } else {
            std::set<std::pair<double, int> >::iterator it = PriorityList.begin();
            std::set<NodeData>::iterator it2 = findNode(it->second);            
            return getNodeVector(it2);
        }
    }

    void pop() {
        /// pop() removes the lowest cost node from the priority queue
        if (isEmpty()) {
        } else {
            std::set<std::pair<double, int> >::iterator it = PriorityList.begin();
            std::set<NodeData>::iterator it2 = findNode(it->second);
            NodeSet.erase(it2);
            PriorityList.erase(it);
        }
    }

    std::set<NodeData>::iterator findNode(int id) {
        /**findNode()  Search the vector container of PriorityList and returns
         * the iterator/position of the node in the vector container.
         * Returns position of last position in vector if node is not present.
         */
        std::set<NodeData>::iterator it = NodeSet.find(id);
        return it;
    }

    bool doesNodeExist(int id) {
        /// doesNodeExist() Search the PriorityList and returns if the node exists or not.
        std::set<NodeData>::const_iterator it = findNode(id);
        return (it != NodeSet.end()) ? true : false;
    }

    std::vector<std::vector<double> > getNodeData(std::vector<int> idList) {
        /**getNodeData()  Search the vector container of PriorityList and
         * the data of the node in the PriorityList. If node is not present
         * then it returns the array of zeros.
         */
        std::vector<std::vector<double> > out;

        for (size_t i = 0; i < idList.size(); i++) {
            std::set<NodeData>::iterator it = findNode(idList[i]);
            
            if (it != NodeSet.end())
                out.push_back(getNodeVector(it));
            else 
                out.push_back(getEmptyNodeVector());            
        }
        return out;
    }
    void replaceNodeData(const double* newNode,int numRows = 1) {
        /// replaceNodeData() Finds the node in PriorityList and replace the old
        /// data with the new data provided. 'newNode' consists of 'numRows' nodes
        /// data to be replaced.
        for (int i = 0; i < numRows; i++) {
            std::set<NodeData>::iterator it = findNode(static_cast<int>(newNode[i]));

            if (it != NodeSet.end()) {
                // To delete the node from PriorityList, we need to locate the
                // position of 'id' in the list.
                std::set<std::pair<double, int> >::iterator it2 =
                    PriorityList.find(std::make_pair(it->fScore, it->id));
                // it2 is the start position of the where the cost is present.
                // Matches the 'id'
                if (it2->second == it->id) {
                    PriorityList.erase(it2);
                    it2 = PriorityList.end();
                }
                NodeSet.erase(it);

                NodeData node;
                // Consolidating data
                node.id = static_cast<int>(newNode[i]);
                node.parent = static_cast<int>(newNode[i + numRows]);
                node.fScore = static_cast<double>(newNode[i + numRows * 2]);
                node.gScore = static_cast<double>(newNode[i + numRows * 3]);
                node.hScore = static_cast<double>(newNode[i + numRows * 4]);

                for (int j = 5; j < DataDimension; j++) {
                    node.nScores.push_back(static_cast<double>(newNode[i + numRows * j]));
                }
                // Inserting the node.
                NodeSet.insert(node);
                PriorityList.insert(std::make_pair(node.fScore, node.id));
            } 
        }
        return;
    }

    std::vector<std::vector<double> > getAllData() {
        /// getAllData() Returns the content of the all the nodes present.
        /// The node data not to be in the same order as stored in PriorityList
        std::vector<std::vector<double> > out;
        
        if(isEmpty()){
            out.push_back(getEmptyNodeVector());
        }
        else{
            std::set<NodeData>::iterator it = NodeSet.begin();            
            for (; it != NodeSet.end(); ++it) {
                out.push_back(getNodeVector(it));
            }
        }                       
        return out;
    }

    std::vector<std::vector<double> > getfScorePriorityQueue() {
        /// getfScorePriorityQueue() Returns the content of the PriorityList (f,node).

        std::vector<std::vector<double> > dataPQ;
        std::set<std::pair<double, int> >::iterator it = PriorityList.begin();
        for (; it != PriorityList.end(); ++it) {
            std::vector<double> currVal;
            currVal.push_back(static_cast<double>(it->second));
            currVal.push_back(static_cast<double>(it->first));
            dataPQ.push_back(currVal);            
        }
        return dataPQ;
    }

   std::vector<int> traceBack(int id) {
        /** traceBack() Returns the path from goal 'id' to start position, by
         * retracing elements.
         *
         * The first element gives the size of the path array.
         */
        std::set<NodeData>::iterator node = findNode(id);
        std::vector<int> path;

        int maxLen = size();
        path.push_back(node->id);
        int len = 1;
        while (node->id != node->parent && len <= maxLen) {
            node = findNode(node->parent);
            if (node != NodeSet.end()) {
                path.push_back(node->id);
                len++;
            } else {
                break;
            }
        }        
        return path;
    }
    /// size() Returns the size of the Priority Queue.
    int size() {
        return static_cast<int>(NodeSet.size());
    }
    /// isEmpty() Checks if queue is empty or not.
    bool isEmpty() {
        return (NodeSet.empty()) ? true : false;
    }
};
} // namespace nav
#endif
