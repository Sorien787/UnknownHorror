// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "../Common/UnrealUtilities.h"
// #include <algorithm>
//
// template< typename T, int AS = 1 >
// class Octree
// {
// public:
//     Octree( int size, const T& emptyValue = T(0) );
//     Octree( const Octree<T,AS>& o );
//     ~Octree();
//
//     // Accessors
//     int size() const;
//     const T& emptyValue() const;
//
//     static unsigned long branchBytes();
//     static unsigned long aggregateBytes();
//     static unsigned long leafBytes();
//     unsigned long bytes() const;
//
//     int nodes() const;
//     int nodesAtSize( int size ) const;
//
//     // Mutators
//     void setEmptyValue( const T& emptyValue );
//
//     void swap( Octree<T,AS>& o );
//     Octree<T,AS>& operator= ( Octree<T,AS> o );
//
//     // Indexing operators
//     T& operator() ( int x, int y, int z );
//     const T& operator() ( int x, int y, int z ) const;
//     const T& at( int x, int y, int z ) const;
//
//     void set( int x, int y, int z, const T& value );
//     void erase( int x, int y, int z );
//
//     // I/O functions
//     void writeBinary( std::ostream& out ) const;
//     void readBinary( std::istream& in );
//
// protected:
//
//     // Octree node types
//     class Node;
//     class Branch;
//     class Aggregate;
//     class Leaf;
//     enum NodeType { BranchNode, AggregateNode, LeafNode };
//
//     Node*& root();
//     const Node* root() const;
//
//     static void deleteNode( Node** node );
//
// private:
//     // Recursive helper functions
//     void eraseRecursive( Node** node, int size, int x, int y, int z );
//     static unsigned long bytesRecursive( const Node* node );
//     static int nodesRecursive( const Node* node );
//     static int nodesAtSizeRecursive( int targetSize, int size, Node* node );
//     void zSliceRecursive( Array2D<T> slice, const Node* node, int size,
//             int x, int y, int z, int targetZ ) const;
//     static void writeBinaryRecursive( std::ostream& out, const Node* node );
//     static void readBinaryRecursive( std::istream& in, Node** node );
//
// protected:
//     // Node classes
//
//     class Node
//     {
//     public:
//         NodeType type() const;
//
//     protected:
//         Node( NodeType type );
//         ~Node() {};
//
//     private:
//         NodeType type_ : 2;
//     };
//
//     class Branch : public Node
//     {
//     public:
//         Branch();
//         Branch( const Branch& b );
//         ~Branch();
//
//         const Node* child( int x, int y, int z ) const;
//         Node*& child( int x, int y, int z );
//         const Node* child( int index ) const;
//         Node*& child( int index );
//
//         friend void Octree<T,AS>::deleteNode( Node** node );
//
//     private:
//         Branch& operator= ( Branch b );
//
//     private:
//         Node* children[2][2][2];
//     };
//
//     class Aggregate : public Node
//     {
//     public:
//         Aggregate( const T& v );
//
//         const T& value( int x, int y, int z ) const;
//         T& value( int x, int y, int z );
//         void setValue( int x, int y, int z, const T& v );
//
//         const T& value( int i ) const;
//         T& value( int i );
//         void setValue( int i, const T& v );
//
//         friend void Octree<T,AS>::deleteNode( Node** node );
//
//     private:
//         ~Aggregate() {};
//
//     private:
//         T value_[AS][AS][AS];
//     };
//
//     class Leaf : public Node
//     {
//     public:
//         Leaf( const T& v );
//
//         const T& value() const;
//         T& value();
//         void setValue( const T& v );
//
//         friend void Octree<T,AS>::deleteNode( Node** node );
//
//     private:
//         ~Leaf() {};
//
//     private:
//         T value_;
//     };
//
//     static const int aggregateSize_ = AS;
//
// private:
//     Node* root_;
//     T emptyValue_;
//     int size_;
// };
//
// enum class Traversibility
// {
//     Air,
//     Solid,
//     DifficultSolid
// };
//
// using NavGrid = Octree<Grid<Traversibility>>;
