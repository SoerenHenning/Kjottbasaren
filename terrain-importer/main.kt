
import java.io.File
import java.io.FileInputStream
import java.io.InputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder

fun main(args: Array<String>) {
    if (args.isEmpty()) {
        throw IllegalArgumentException("No binary file given.")
    }
    val inputStream = FileInputStream(args[0])

    val numberOfColumns = inputStream.readInt()
    val numberOfRows = inputStream.readInt()
    val lowerLeftXCoord = inputStream.readDouble()
    val lowerLeftYCoord = inputStream.readDouble()
    val cellSize = inputStream.readDouble()
    val noData = inputStream.readInt()

    val numberOfPoints = numberOfColumns * numberOfRows
    val heightsConsecutive = FloatArray(numberOfPoints)
    heightsConsecutive.indices.forEach { heightsConsecutive[it] = inputStream.readFloat() }

    //val heights = Array(numberOfColumns, { FloatArray(numberOfRows) })
    //heightsConsecutive.withIndex().forEach { (i,v) -> heights[i / numberOfRows][i % numberOfRows] }
    val vertices = heightsConsecutive.withIndex().map { (index, value) -> Vector(
            (index % numberOfRows) * cellSize + lowerLeftXCoord,
            value.toDouble(),
            (index / numberOfRows) * cellSize + lowerLeftYCoord
    ) }

    fun coordinatesToIndex(column: Int, row: Int) = column * numberOfRows + row

    val triangles = (0 until numberOfColumns - 1).map { column ->
        (0 until numberOfRows - 1).map { row -> run {
            val i0 = coordinatesToIndex(column, row)
            val v0 = vertices[i0]
            val i1 = coordinatesToIndex(column + 1, row)
            val v1 = vertices[i1]
            val i2 = coordinatesToIndex(column, row + 1)
            val v2 = vertices[i2]
            val i3 = coordinatesToIndex(column + 1, row + 1)
            val v3 = vertices[i3]
            if (v0.distanceTo(v3) < v1.distanceTo(v2)) {
                listOf(Triple(i0, i1, i3), Triple(i0, i3, i2))
            } else {
                listOf(Triple(i1, i2, i0), Triple(i1, i3, i2))
            }
        }}
    }.flatten().flatten()

    val triangleNormals = triangles.map {
        (vertices[it.second] - vertices[it.first]).cross(vertices[it.third] - vertices[it.first])
    }

    // Map: Vertex Index -> List of Triangles
    val trianglesPerVertex = mutableMapOf<Int, MutableList<Int>>()
    triangles.forEachIndexed { index, triangle ->
        trianglesPerVertex.getOrPut(triangle.first, { mutableListOf() }).add(index)
        trianglesPerVertex.getOrPut(triangle.second, { mutableListOf() }).add(index)
        trianglesPerVertex.getOrPut(triangle.third, { mutableListOf() }).add(index)
    }

    val vertexNormals = trianglesPerVertex.map {
        it.value.fold(Vector.zero, { sum, next -> sum + triangleNormals[next] }).normalize()
    }

    val textureCoordinates = vertices.indices.map {
        val column = (it / numberOfRows)
        val row = (it % numberOfRows)
        val s = column / (numberOfColumns - 1.0)
        val t = 1.0 - (row / (numberOfRows - 1.0))
        Pair(s,t)
    }

    val trianglesWithoutNoData = triangles.filterNot { vertices[it.first].y <= noData || vertices[it.second].y <= noData || vertices[it.third].y <= noData }

    println("Cols: $numberOfColumns")
    println("numberOfRows: $numberOfRows")
    println("lowerLeftXCoord: $lowerLeftXCoord")
    println("lowerLeftYCoord: $lowerLeftYCoord")
    println("cellSize: $cellSize")
    println("noData: $noData")

    val printWriter = File("terrain.obj").printWriter()

    printWriter.println("mtllib terrain.mtl")
    printWriter.println("o Terrain")

    for (vertex in vertices) {
        printWriter.println("v ${vertex.x} ${vertex.y} ${vertex.z}")
    }

    for (normal in vertexNormals) {
        printWriter.println("vn ${normal.x} ${normal.y} ${normal.z}")
    }

    for (coord in textureCoordinates) {
        printWriter.println("vt ${coord.first} ${coord.second}")
    }

    printWriter.println("usemtl Bergen")

    for (triangle in trianglesWithoutNoData) {
        printWriter.println("f ${triangle.first + 1}/${triangle.first + 1}/${triangle.first + 1} ${triangle.second + 1}/${triangle.second + 1}/${triangle.second + 1} ${triangle.third + 1}/${triangle.third + 1}/${triangle.third + 1}")
    }

    printWriter.close()
}

private data class Vector(val x: Double, val y: Double, val z: Double) {

    companion object {
        val zero = Vector(0.0, 0.0,0.0)
    }

    fun distanceTo(other: Vector) : Double {
        return Math.sqrt(Math.pow(this.x - other.x, 2.0) + Math.pow(this.y - other.y, 2.0) + Math.pow(this.z - other.z, 2.0))
    }

    operator fun plus(other: Vector) : Vector {
        return Vector(
                this.x + other.x,
                this.y + other.y,
                this.z + other.z
        )
    }
    
    operator fun minus(other: Vector) : Vector {
        return Vector(
                this.x - other.x,
                this.y - other.y,
                this.z - other.z
        )
    }

    fun cross(other: Vector) : Vector {
        return Vector(
                this.y * other.z - this.z * other.y,
                this.z * other.x - this.x * other.z,
                this.x * other.y - this.y * other.x
        )
    }

    fun normalize() : Vector {
        val length = this.length()
        return Vector(this.x / length, this.y / length, this.z / length)
    }

    fun length(): Double {
        return Math.sqrt(Math.pow(this.x, 2.0) + Math.pow(this.y, 2.0) + Math.pow(this.z, 2.0))
    }
}

private fun InputStream.readInt(): Int {
    val bytes = ByteBuffer.allocate(4)
    this.read(bytes.array())
    return bytes.order(ByteOrder.LITTLE_ENDIAN).int
}

private fun InputStream.readDouble(): Double {
    val bytes = ByteBuffer.allocate(8)
    this.read(bytes.array())
    return bytes.order(ByteOrder.LITTLE_ENDIAN).double
}

private fun InputStream.readFloat(): Float {
    val bytes = ByteBuffer.allocate(4)
    this.read(bytes.array())
    return bytes.order(ByteOrder.LITTLE_ENDIAN).float
}