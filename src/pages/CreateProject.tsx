import { useState, FormEvent } from "react";
import { useNavigate, Link } from "react-router-dom";
import { apiFetch } from "../api/client";

interface Requirement {
  text: string;
  mandatory: boolean;
}

interface Criterion {
  name: string;
  weight_percent: number;
}

const DEFAULT_CRITERIA: Criterion[] = [
  { name: "Technology", weight_percent: 25 },
  { name: "Cost", weight_percent: 20 },
  { name: "Capacity", weight_percent: 20 },
  { name: "Certification", weight_percent: 15 },
  { name: "Experience", weight_percent: 10 },
  { name: "Delivery", weight_percent: 10 },
];

export default function CreateProject() {
  const [title, setTitle] = useState("");
  const [quantity, setQuantity] = useState(0);
  const [budget, setBudget] = useState(0);
  const [deliveryMonths, setDeliveryMonths] = useState(0);
  const [requirements, setRequirements] = useState<Requirement[]>([
    { text: "", mandatory: true },
  ]);
  const [criteria, setCriteria] = useState<Criterion[]>(DEFAULT_CRITERIA);
  const [error, setError] = useState("");
  const navigate = useNavigate();

  const weightSum = criteria.reduce((sum, c) => sum + Number(c.weight_percent), 0);

  async function handleCreate(e: FormEvent) {
    e.preventDefault();
    setError("");

    if (Math.abs(weightSum - 100) > 0.01) {
      setError(`Criteria weights must sum to 100 (currently ${weightSum})`);
      return;
    }

    try {
      const created = await apiFetch<{ id: number }>("/projects", {
        method: "POST",
        body: JSON.stringify({
          title,
          quantity,
          budget,
          delivery_months: deliveryMonths,
          requirements: requirements.filter((r) => r.text.trim() !== ""),
        }),
      });

      // Criteria set separately (Part 5's PATCH /criteria endpoint) —
      // matches the locked API contract, even though the UI submits
      // both in one form action.
      await apiFetch(`/projects/${created.id}/criteria`, {
        method: "PATCH",
        body: JSON.stringify(criteria),
      });

      navigate("/government");
    } catch (err) {
      setError((err as Error).message);
    }
  }

  return (
    <div style={{ maxWidth: 600, margin: "40px auto", fontFamily: "sans-serif" }}>
      <div style={{ marginBottom: 16 }}>
        <Link to="/government">← Back to Dashboard</Link>
      </div>
      <h2>Create Procurement Project</h2>
      <form onSubmit={handleCreate}>
        <div style={{ marginBottom: 12 }}>
          <label style={{ display: "block", marginBottom: 4 }}>Title</label>
          <input
            placeholder="e.g. Surveillance Drone Fleet"
            value={title}
            onChange={(e) => setTitle(e.target.value)}
            style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
            required
          />
        </div>
        <div style={{ display: "flex", gap: 12, marginBottom: 12 }}>
          <div style={{ flex: 1 }}>
            <label style={{ display: "block", marginBottom: 4 }}>Quantity (Units)</label>
            <input
              placeholder="Quantity"
              type="number"
              value={quantity || ""}
              onChange={(e) => setQuantity(Number(e.target.value))}
              style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
              required
            />
          </div>
          <div style={{ flex: 1 }}>
            <label style={{ display: "block", marginBottom: 4 }}>Budget (₹)</label>
            <input
              placeholder="Budget (₹)"
              type="number"
              value={budget || ""}
              onChange={(e) => setBudget(Number(e.target.value))}
              style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
              required
            />
          </div>
          <div style={{ flex: 1 }}>
            <label style={{ display: "block", marginBottom: 4 }}>Delivery (months)</label>
            <input
              placeholder="Delivery (months)"
              type="number"
              value={deliveryMonths || ""}
              onChange={(e) => setDeliveryMonths(Number(e.target.value))}
              style={{ width: "100%", padding: 8, boxSizing: "border-box" }}
              required
            />
          </div>
        </div>

        <h4>Requirements</h4>
        {requirements.map((r, i) => (
          <div key={i} style={{ display: "flex", gap: 8, alignItems: "center", marginBottom: 8 }}>
            <input
              placeholder="Requirement text"
              value={r.text}
              onChange={(e) => {
                const next = [...requirements];
                next[i].text = e.target.value;
                setRequirements(next);
              }}
              style={{ flex: 1, padding: 6 }}
            />
            <label style={{ display: "flex", alignItems: "center", gap: 4 }}>
              <input
                type="checkbox"
                checked={r.mandatory}
                onChange={(e) => {
                  const next = [...requirements];
                  next[i].mandatory = e.target.checked;
                  setRequirements(next);
                }}
              />
              Mandatory
            </label>
          </div>
        ))}
        <button
          type="button"
          onClick={() => setRequirements([...requirements, { text: "", mandatory: true }])}
          style={{ padding: "4px 8px", cursor: "pointer", marginBottom: 16 }}
        >
          + Add requirement
        </button>

        <h4>Evaluation Criteria (weights must total 100%)</h4>
        <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 8, marginBottom: 12 }}>
          {criteria.map((c, i) => (
            <div key={i} style={{ display: "flex", alignItems: "center", gap: 6 }}>
              <span style={{ width: 100 }}>{c.name}:</span>
              <input
                type="number"
                value={c.weight_percent}
                onChange={(e) => {
                  const next = [...criteria];
                  next[i].weight_percent = Number(e.target.value);
                  setCriteria(next);
                }}
                style={{ width: 60, padding: 4 }}
              />
              <span>%</span>
            </div>
          ))}
        </div>
        <p style={{ fontWeight: "bold", color: Math.abs(weightSum - 100) < 0.01 ? "green" : "red" }}>
          Total: {weightSum}%
        </p>

        <button type="submit" style={{ padding: "8px 20px", cursor: "pointer", marginTop: 8 }}>
          Create Project
        </button>
        {error && <p style={{ color: "red", marginTop: 8 }}>{error}</p>}
      </form>
    </div>
  );
}
