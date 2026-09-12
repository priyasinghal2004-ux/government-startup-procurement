import { useEffect, useState } from "react";
import { Link } from "react-router-dom";
import { apiFetch } from "../api/client";
import { useAuth } from "../context/AuthContext";

interface Project {
  id: number;
  title: string;
  status: string;
  budget: number;
  quantity: number;
}

export default function GovernmentDashboard() {
  const [projects, setProjects] = useState<Project[]>([]);
  const { logout } = useAuth();

  useEffect(() => {
    apiFetch<Project[]>("/projects").then(setProjects).catch(console.error);
  }, []);

  async function handlePublish(projectId: number) {
    try {
      await apiFetch(`/projects/${projectId}/publish`, { method: "POST" });
      const updated = await apiFetch<Project[]>("/projects");
      setProjects(updated);
    } catch (err) {
      alert((err as Error).message || "Failed to publish project");
    }
  }

  return (
    <div style={{ maxWidth: 750, margin: "40px auto", fontFamily: "sans-serif" }}>
      <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
        <h2>Government Dashboard</h2>
        <button onClick={logout} style={{ padding: "4px 12px", cursor: "pointer" }}>Logout</button>
      </div>

      <div style={{ margin: "16px 0" }}>
        <Link to="/government/projects/new" style={{ fontWeight: "bold" }}>+ Create New Project</Link>
      </div>

      <h3>My Projects</h3>
      <table style={{ width: "100%", marginTop: 16, borderCollapse: "collapse" }} border={1} cellPadding={8}>
        <thead>
          <tr style={{ background: "#f0f0f0" }}>
            <th align="left">Title</th>
            <th align="left">Status</th>
            <th align="left">Budget</th>
            <th align="left">Actions</th>
          </tr>
        </thead>
        <tbody>
          {projects.length === 0 ? (
            <tr>
              <td colSpan={4} align="center" style={{ padding: 20, color: "#666" }}>
                No projects found. Create your first project above.
              </td>
            </tr>
          ) : (
            projects.map((p) => (
              <tr key={p.id}>
                <td>{p.title}</td>
                <td>
                  <span style={{
                    padding: "2px 6px",
                    borderRadius: 4,
                    background: p.status === "OPEN" ? "#d4edda" : "#fff3cd",
                    color: p.status === "OPEN" ? "#155724" : "#856404"
                  }}>
                    {p.status}
                  </span>
                </td>
                <td>₹{p.budget.toLocaleString()}</td>
                <td>
                  <Link to={`/projects/${p.id}`}>View</Link>
                  {" | "}
                  <Link to={`/projects/${p.id}/rankings`}>Rankings</Link>
                  {p.status === "DRAFT" && (
                    <>
                      {" | "}
                      <button
                        onClick={() => handlePublish(p.id)}
                        style={{ padding: "2px 8px", cursor: "pointer" }}
                      >
                        Publish
                      </button>
                    </>
                  )}
                </td>
              </tr>
            ))
          )}
        </tbody>
      </table>
    </div>
  );
}
