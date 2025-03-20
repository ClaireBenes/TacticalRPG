// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalRPG/Utility/Pathfinding.h"

// Sets default values
APathfinding::APathfinding()
{
	PrimaryActorTick.bCanEverTick = false;
}

TArray<FVector2D> APathfinding::FindPath(FVector2D Start, FVector2D Goal, const TSet<FVector2D>& Obstacles, int GridSizeX, int GridSizeY)
{
    TArray<TSharedPtr<FPathNode>> OpenSet;
    TSet<FVector2D> ClosedSet;

    TSharedPtr<FPathNode> StartNode = MakeShared<FPathNode>(Start);
    StartNode->GCost = 0;
    StartNode->HCost = CalculateHeuristic(Start, Goal);
    OpenSet.Add(StartNode);

    while (OpenSet.Num() > 0)
    {
        // Get node with lowest F cost
        OpenSet.Sort([](const TSharedPtr<FPathNode>& A, const TSharedPtr<FPathNode>& B) { return A->FCost() < B->FCost(); });
        TSharedPtr<FPathNode> CurrentNode = OpenSet[0];
        OpenSet.RemoveAt(0);
        ClosedSet.Add(CurrentNode->Position);

        if (CurrentNode->Position == Goal)
        {
            return ReconstructPath(CurrentNode);
        }

        // Possible movements (including diagonal)
        FVector2D Neighbors[] = {
            FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1), // Cardinal directions
            FVector2D(1, 1), FVector2D(1, -1), FVector2D(-1, 1), FVector2D(-1, -1) // Diagonals
        };

        for (FVector2D Direction : Neighbors)
        {
            FVector2D NeighborPos = CurrentNode->Position + Direction;

            // Skip if out of bounds or in the obstacle set
            if (NeighborPos.X < -GridSizeX || NeighborPos.Y < -GridSizeY || NeighborPos.X >= GridSizeX || NeighborPos.Y >= GridSizeY || Obstacles.Contains(NeighborPos))
            {
                continue;
            }

            if (ClosedSet.Contains(NeighborPos)) continue;

            float NewGCost = CurrentNode->GCost + (Direction.X != 0 && Direction.Y != 0 ? 14 : 10); // Diagonal = 14, Straight = 10

            TSharedPtr<FPathNode> NeighborNode = MakeShared<FPathNode>(NeighborPos);
            NeighborNode->GCost = NewGCost;
            NeighborNode->HCost = CalculateHeuristic(NeighborPos, Goal);
            NeighborNode->Parent = CurrentNode;

            bool bBetterPath = true;
            for (const TSharedPtr<FPathNode>& OpenNode : OpenSet)
            {
                if (OpenNode->Position == NeighborPos && OpenNode->GCost <= NewGCost)
                {
                    bBetterPath = false;
                    break;
                }
            }

            if (bBetterPath)
            {
                OpenSet.Add(NeighborNode);
            }
        }
    }

    return {}; // No path found
}

float APathfinding::CalculateHeuristic(FVector2D Node, FVector2D Target)
{
    return 10 * (FMath::Abs(Node.X - Target.X) + FMath::Abs(Node.Y - Target.Y)); // Manhattan Distance
}

TArray<FVector2D> APathfinding::ReconstructPath(TSharedPtr<FPathNode> Node)
{
    TArray<FVector2D> Path;
    while (Node.IsValid())
    {
        Path.Add(Node->Position);
        Node = Node->Parent;
    }
    Algo::Reverse(Path);
    return Path;
}

