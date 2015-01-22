using PricerWrapper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace WebSite.Models
{
    public class FunctionsPricer
    {
        private WrapperClass wc;
        private KozeiDBEntities _db = new KozeiDBEntities();
        public FunctionsPricer()
        {
            wc = new WrapperClass();
        }

        public void getPrice(out double prix, out double ic) 
        {
            wc.getPrice_0();
            prix = wc.getPrice(); //prix en 0
            ic = wc.getConfidence();
        }

        public void getPrice(out double prix, out double ic, double t)
        {
            wc.getPrice_t(t);
            prix = wc.getPrice(); //prix en t
            ic = wc.getConfidence();
        }

        void viderDB()
        {
            //_db.Database.ExecuteSqlCommand("ALTER TABLE Portfolio_Part DROP CONSTRAINT FK_Portfolio_Part_ToFinancial_Product;" +
            //                               "ALTER TABLE Portfolio_Part DROP CONSTRAINT FK_Portfolio_Part_ToPortfolio;" +
            //                               "ALTER TABLE Value DROP CONSTRAINT FK_Value_ToFinancial_Product;" +
            //                               "ALTER TABLE Value DROP CONSTRAINT FK_Value_ToModele;" +
            //                               "ALTER TABLE Value DROP CONSTRAINT FK_Value_ToPortfolio;" +
            //                               "TRUNCATE TABLE Value; " + //vider les prix
            //                               "TRUNCATE TABLE Portfolio_Part;" + //vider les valeurs de delta
            //                               "ALTER TABLE Value ADD CONSTRAINT [FK_Value_ToFinancial_Product] FOREIGN KEY ([financial_product_id]) REFERENCES [dbo].[Financial_Product] ([Id]);" +
            //                               "ALTER TABLE Value ADD CONSTRAINT [FK_Value_ToModele] FOREIGN KEY ([error_modele_id]) REFERENCES [dbo].[Modele] ([Id]);" +
            //                               "ALTER TABLE Value ADD  CONSTRAINT [FK_Value_ToPortfolio] FOREIGN KEY ([price_portfolio_id]) REFERENCES [dbo].[Portfolio] ([Id]);" +
            //                               "ALTER TABLE Portfolio_Part ADD CONSTRAINT [FK_Portfolio_Part_ToPortfolio] FOREIGN KEY ([portfolio_id]) REFERENCES [dbo].[Portfolio] ([Id]);" +
            //                               "ALTER TABLE Portfolio_Part ADD CONSTRAINT [FK_Portfolio_Part_ToFinancial_Product] FOREIGN KEY ([financial_product_id]) REFERENCES [dbo].[Financial_Product] ([Id]);"
            //                               );

            // On vide Portfolio_Part
            List<Portfolio_Part> listPP = _db.Portfolio_Part.Where(pp => pp.portfolio_id == 1).ToList();
            foreach (Portfolio_Part pp in listPP)
                _db.Portfolio_Part.Remove(pp);

            // On vide les valeurs
            List<Value> listVal = _db.Value.Where(v => v.price_portfolio_id == 1 || v.error_modele_id == 1).ToList();
            foreach (Value v in listVal)
                _db.Value.Remove(v);

            // Commit
            _db.SaveChanges();
        }

        public void hedge(int rebal)
        {
            //vidage de la couverture 
            viderDB();

            int n = 30;
            double[] portfolio = new double[3*(rebal+1)];
            double PnL = 0;
            double px = 0;
            double[] delta = new double[(rebal+1)*30];
            wc.hedge_T(rebal, n, portfolio, PnL, px, delta);

            PnL = wc.getPnL();
            px = wc.getPrice();

            Modele m = _db.Modele.First(X => X.Id == 1);

            //ajout de l'erreur de couverture
            for (int i=0; i< rebal + 1; i++)
            {
                for (int j = 0; j < 30; j++)
                {
                    Portfolio_Part p = new Portfolio_Part(); //faire un constructeur
                    p.date = ((double)i / rebal) * 8;
                    p.part = delta[i * 30 + j];
                    p.financial_product_id = j+1;
                    p.portfolio_id = 1;
                    _db.Portfolio_Part.Add(p);
                }
            }
            // TODO : mettre la bonne date
            Value v1 = new Value(px, 0, null, 1, null, null);
            Value v2 = new Value(PnL, 0, null, null, 1, null);
            _db.Value.Add(v1);
            _db.Value.Add(v2);

            // Commit
            _db.SaveChanges();
        }

    }
}