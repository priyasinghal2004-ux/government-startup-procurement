import { useEffect, useState, FormEvent } from "react";
import { useParams, Link } from "react-router-dom";
import { apiFetch } from "../api/client";
import { useAuth } from "../context/AuthContext";

interface Requirement { text: string; mandatory: boolean; }
interface Criterion { name: string; weight_percent: number; }
interface ProjectDetail {
  id: number; title: string; description: string;
  quantity: number; budget: number; delivery_months: number;
  status: string; requirements: Requirement[]; criteria: Criterion[];
}

export default function ProjectDetails() {
  const { id } = useParams();
  const { role } = useAuth();
  const [project, setProject] = useState<ProjectDetail | null>(null);
  const [applied, setApplied] = useState(false);
  const [eligibilityResult, setEligibilityResult] = useState<string | null>(null);
  const [error, setError] = useState<string | null>(null);

  const [form, setForm] = useState({
    quoted_cost: 0,
    production_capacity: 0,
    technology_score: 0,
    certification_status: false,
    experience_years: 0,
    delivery_months: 0,
  });

  useEffect(() => {
    apiFetch<ProjectDetail>(`/projects/${id}`).then(setProject).catch(console.error);
  }, [id]);

  async function handleApply(e: FormEvent) {
    e.preventDefault();
    setError(null);
    try {
      const res = await apiFetch<{ id: number; eligibility_status: string }>(`/projects/${id}/applications`, {
        method: "POST",
        body: JSON.stringify(form),
      });
      setApplied(true);
      setEligibilityResult(res.eligibility_status);
    } catch (err) {
      setError((err as Error).message || "Submission failed");
    }
  }

  if (!project) return <div style={{ maxWidth: 600, margin: "40px auto", fontFamily: "sans-serif" }}>Loading...</div>;

  return (
    <div style={{ maxWidth: 650, margin: "40px auto", fontFamily: "sans-serif" }}>
      <div style={{ marginBottom: 16 }}>
        <Link to={role === "government" ? "/government" : "/startup"}>
          ← Back to {role === "government" ? "Government Dashboard" : "Available Projects"}
        </Link>
      </div>

      <h2>{project.title}</h2>
      {project.description && <p>{project.description}</p>}

      <div style={{ background: "#f8f9fa", padding: 12, borderRadius: 6, margin: "16px 0" }}>
        <p style={{ margin: "4px 0" }}>
          <strong>Status:</strong> {project.status} | <strong>Budget:</strong> ₹{project.budget.toLocaleString()}
        </p>
        <p style={{ margin: "4px 0" }}>
          <strong>Required Quantity:</strong> {project.quantity} units | <strong>Target Delivery:</strong> {project.delivery_months} months
        </p>
      </div>

      <h4>Mandatory Requirements Checklist</h4>
      {project.requirements && project.requirements.length > 0 ? (
        <ul>
          {project.requirements.map((r, i) => (
            <li key={i}>{r.text} {r.mandatory && <strong style={{ color: "#d9534f" }}>(mandatory)</strong>}</li>
          ))}
        </ul>
      ) : (
        <p style={{ color: "#666" }}>No specific checklist requirements declared.</p>
      )}

      <h4>Evaluation Criteria Breakdown</h4>
      {project.criteria && project.criteria.length > 0 ? (
        <ul>
          {project.criteria.map((c, i) => (
            <li key={i}><strong>{c.name}</strong>: {c.weight_percent}%</li>
          ))}
        </ul>
      ) : (
        <p style={{ color: "#666" }}>Criteria not yet defined.</p>
      )}

      {role === "government" && (
        <div style={{ marginTop: 24 }}>
          <Link
            to={`/projects/${project.id}/rankings`}
            style={{
              display: "inline-block",
              background: "#28a745",
              color: "#fff",
              padding: "8px 16px",
              borderRadius: 4,
              textDecoration: "none"
            }}
          >
            View Applications & Ranked Leaderboard →
          </Link>
        </div>
      )}

      {role === "startup" && project.status === "OPEN" && !applied && (
        <div style={{ marginTop: 28, borderTop: "2px solid #eee", paddingTop: 20 }}>
          <h3>Submit Application Proposal</h3>
          <p style={{ fontSize: 13, color: "#666" }}>
            Note: Applications undergo immediate eligibility gating (Certification == true, Production Capacity &ge; Quantity, Technology Score &ge; 50).
          </p>
          <form onSubmit={handleApply}>
            <div style={{ marginBottom: 10 }}>
              <label style={{ display: "block", marginBottom: 4 }}>Quoted Cost (₹)</label>
              <input
                type="number"
                value={form.quoted_cost || ""}
                onChange={(e) => setForm({ ...form, quoted_cost: Number(e.target.value) })}
                style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
                required
              />
            </div>
            <div style={{ marginBottom: 10 }}>
              <label style={{ display: "block", marginBottom: 4 }}>Production Capacity (Units)</label>
              <input
                type="number"
                value={form.production_capacity || ""}
                onChange={(e) => setForm({ ...form, production_capacity: Number(e.target.value) })}
                style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
                required
              />
            </div>
            <div style={{ marginBottom: 10 }}>
              <label style={{ display: "block", marginBottom: 4 }}>Technology Score (0-100)</label>
              <input
                type="number"
                value={form.technology_score || ""}
                onChange={(e) => setForm({ ...form, technology_score: Number(e.target.value) })}
                style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
                required
              />
            </div>
            <div style={{ marginBottom: 10 }}>
              <label style={{ display: "flex", alignItems: "center", gap: 8, cursor: "pointer" }}>
                <input
                  type="checkbox"
                  checked={form.certification_status}
                  onChange={(e) => setForm({ ...form, certification_status: e.target.checked })}
                />
                Possess Required Defense/Industry Certifications
              </label>
            </div>
            <div style={{ marginBottom: 10 }}>
              <label style={{ display: "block", marginBottom: 4 }}>Experience (years)</label>
              <input
                type="number"
                value={form.experience_years || ""}
                onChange={(e) => setForm({ ...form, experience_years: Number(e.target.value) })}
                style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
                required
              />
            </div>
            <div style={{ marginBottom: 16 }}>
              <label style={{ display: "block", marginBottom: 4 }}>Committed Delivery (months)</label>
              <input
                type="number"
                value={form.delivery_months || ""}
                onChange={(e) => setForm({ ...form, delivery_months: Number(e.target.value) })}
                style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
                required
              />
            </div>
            <button
              type="submit"
              style={{
                background: "#0066cc",
                color: "#fff",
                padding: "8px 20px",
                borderRadius: 4,
                border: "none",
                cursor: "pointer"
              }}
            >
              Submit Application
            </button>
            {error && <p style={{ color: "red", marginTop: 8 }}>{error}</p>}
          </form>
        </div>
      )}

      {applied && (
        <div style={{
          marginTop: 20,
          padding: 16,
          borderRadius: 6,
          background: eligibilityResult === "ELIGIBLE" ? "#d4edda" : "#f8d7da",
          color: eligibilityResult === "ELIGIBLE" ? "#155724" : "#721c24"
        }}>
          <h4>Application Submitted!</h4>
          <p>
            <strong>Eligibility Status:</strong> {eligibilityResult}
          </p>
          {eligibilityResult === "ELIGIBLE" ? (
            <p>Your proposal successfully passed all mandatory baseline gates and is entered into the ranking engine.</p>
          ) : (
            <p>Your proposal did not meet one or more mandatory eligibility requirements (certification, capacity, or technology baseline).</p>
          )}
        </div>
      )}
    </div>
  );
}
