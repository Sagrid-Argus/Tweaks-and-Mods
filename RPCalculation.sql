DROP TEMPORARY TABLE IF EXISTS tmp_players_cp;
DROP TEMPORARY TABLE IF EXISTS tmp_ranked_faction;
DROP TEMPORARY TABLE IF EXISTS tmp_faction_counts;
DROP TEMPORARY TABLE IF EXISTS tmp_bracket_counts;
DROP TEMPORARY TABLE IF EXISTS tmp_bracket_min_rank;
DROP TEMPORARY TABLE IF EXISTS brackets;
DROP TEMPORARY TABLE IF EXISTS tmp_bracket_rp;

CREATE TEMPORARY TABLE tmp_players_cp AS
SELECT
    c.guid,
    c.name,
    CASE 
        WHEN c.race IN (1,3,4,7) THEN 'ALLIANCE'
        WHEN c.race IN (2,5,6,8) THEN 'HORDE'
        ELSE 'UNKNOWN'
    END AS faction,
    SUM(h.honor) AS total_cp,
    COUNT(*) AS total_hk
FROM characters c
JOIN character_honor_cp h ON h.guid = c.guid
GROUP BY c.guid, c.name, faction;

CREATE TEMPORARY TABLE tmp_faction_counts AS
SELECT
    faction,
    COUNT(*) AS total_players
FROM tmp_players_cp
GROUP BY faction;

CREATE TEMPORARY TABLE tmp_ranked_faction AS
SELECT
    t.*,
    (@r := IF(@prev = t.faction, @r + 1, 1)) AS rank_in_faction,
    (@prev := t.faction) AS _
FROM (
    SELECT *
    FROM tmp_players_cp
    ORDER BY faction, total_cp DESC
) t,
(SELECT @r := 0, @prev := '') vars;

CREATE TEMPORARY TABLE brackets AS
SELECT
    r.*,
    fc.total_players,
    CASE
        WHEN r.rank_in_faction <= fc.total_players * 0.002 THEN 14
        WHEN r.rank_in_faction <= fc.total_players * 0.007 THEN 13
        WHEN r.rank_in_faction <= fc.total_players * 0.017 THEN 12
        WHEN r.rank_in_faction <= fc.total_players * 0.037 THEN 11
        WHEN r.rank_in_faction <= fc.total_players * 0.077 THEN 10
        WHEN r.rank_in_faction <= fc.total_players * 0.137 THEN 9
        WHEN r.rank_in_faction <= fc.total_players * 0.207 THEN 8
        WHEN r.rank_in_faction <= fc.total_players * 0.287 THEN 7
        WHEN r.rank_in_faction <= fc.total_players * 0.377 THEN 6
        WHEN r.rank_in_faction <= fc.total_players * 0.477 THEN 5
        WHEN r.rank_in_faction <= fc.total_players * 0.587 THEN 4
        WHEN r.rank_in_faction <= fc.total_players * 0.715 THEN 3
        WHEN r.rank_in_faction <= fc.total_players * 0.858 THEN 2
        ELSE 1
    END AS bracket
FROM tmp_ranked_faction r
JOIN tmp_faction_counts fc ON fc.faction = r.faction;

CREATE TEMPORARY TABLE tmp_bracket_counts AS
SELECT
    faction,
    bracket,
    COUNT(*) AS players_in_bracket
FROM brackets
GROUP BY faction, bracket;

CREATE TEMPORARY TABLE tmp_bracket_min_rank AS
SELECT
    faction,
    bracket,
    MIN(rank_in_faction) AS min_rank
FROM brackets
GROUP BY faction, bracket;

CREATE TEMPORARY TABLE tmp_bracket_rp (
    bracket INT,
    rp_max INT,
    rp_min INT
);

INSERT INTO tmp_bracket_rp VALUES
(14,13000,12000),
(13,12000,11000),
(12,11000,10000),
(11,10000,9000),
(10,9000,8000),
(9,8000,7000),
(8,7000,6000),
(7,6000,5000),
(6,5000,4000),
(5,4000,3000),
(4,3000,2000),
(3,2000,1000),
(2,1000,500),
(1,500,100);

SELECT
    b.guid,
    b.name,
    b.faction,
    b.total_cp,
    b.total_hk,
    b.rank_in_faction,
    b.bracket,
    br.rp_max,
    br.rp_min,
    bc.players_in_bracket,
    bm.min_rank,
    CASE 
        WHEN bc.players_in_bracket > 1 THEN
            CEILING(
                br.rp_max - (
                    (b.rank_in_faction - bm.min_rank)
                    * (br.rp_max - br.rp_min)
                    / (bc.players_in_bracket - 1)
                )
            )
        ELSE br.rp_max
    END AS rp_earned
FROM brackets b
JOIN tmp_bracket_rp br ON br.bracket = b.bracket
JOIN tmp_bracket_counts bc
    ON bc.faction = b.faction
   AND bc.bracket = b.bracket
JOIN tmp_bracket_min_rank bm
    ON bm.faction = b.faction
   AND bm.bracket = b.bracket
ORDER BY b.faction, b.rank_in_faction;

