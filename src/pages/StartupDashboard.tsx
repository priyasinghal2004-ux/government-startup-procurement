import { useEffect, useState } from "react";
import { Link } from "react-router-dom";
import { apiFetch } from "../api/client";
import { useAuth } from "../context/AuthContext";

interface Project {
  id: number;
  title: string;
  budget: number;
  quantity: number;
}

export default function StartupDashboard() {
  const [projects, setProjects] = useState<Project[]>([]);
  const { logout } = useAuth();

  useEffect(() => {
    apiFetch<Project[]>("/projects?status=OPEN").then(setProjects).catch(console.error);
  }, []);

  return (
    <div style={{ maxWidth: 700, margin: "40px auto", fontFamily: "sans-serif" }}>
      <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
        <h2>Available Government Projects</h2>
        <button onClick={logout} style={{ padding: "4px 12px", cursor: "pointer" }}>Logout</button>
      </div>

      <p style={{ color: "#666" }}>Open solicitations ready for evaluation and proposal submission:</p>

      {projects.length === 0 ? (
        <p style={{ color: "#666" }}>No open projects currently available.</p>
      ) : (
        <ul style={{ listStyle: "none", padding: 0 }}>
          {projects.map((p) => (
            <li
              key={p.id}
              style={{
                border: "1px solid #ccc",
                borderRadius: 6,
                padding: 14,
                marginBottom: 10,
                display: "flex",
                justifyContent: "space-between",
                alignItems: "center"
              }}
            >
              <div>
                <Link to={`/projects/${p.id}`} style={{ fontWeight: "bold", fontSize: 18 }}>
                  {p.title}
                </Link>
                <div style={{ color: "#555", fontSize: 14, marginTop: 4 }}>
                  Required Quantity: {p.quantity} units | Budget: ₹{p.budget.toLocaleString()}
                </div>
              </div>
              <Link
                to={`/projects/${p.id}`}
                style={{
                  background: "#0066cc",
                  color: "#fff",
                  padding: "6px 14px",
                  borderRadius: 4,
                  textDecoration: "none"
                }}
              >
                View & Apply
              </Link>
            </li>
          ))}
        </ul>
      )}
    </div>
  );
}
