FE_REQUEST_QUERY = """
SELECT
	n1.id,
	n1.x as x1, n1.y as y1,
    n2.x as x2, n2.y as y2,
    n3.x as x3, n3.y as y3,
    n3.props
FROM
	(SELECT
     		elements.id,
			nodes.x,
			nodes.y
		FROM
			elements
			JOIN nodes ON nodes.id = elements.n1
	) n1 JOIN
    (SELECT
     		elements.id,
			nodes.x,
			nodes.y
		FROM
			elements
			JOIN nodes ON nodes.id = elements.n2
	) n2 ON n1.id = n2.id
    JOIN
    (SELECT
     		elements.id,
			nodes.x,
			nodes.y,
     		elements.props
		FROM
			elements
			JOIN nodes ON nodes.id = elements.n3
	) n3 ON n1.id = n3.id
"""
