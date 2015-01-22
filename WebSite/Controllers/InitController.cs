using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using WebSite.Models;

namespace WebSite.Controllers
{
    public class InitController : Controller
    {
        private KozeiDBEntities _db = new KozeiDBEntities();

        public ActionResult Index()
        {
            _db.Database.ExecuteSqlCommand("ALTER TABLE Share DROP CONSTRAINT FK_Share_ToFinancial_Product;" +
                                            "ALTER TABLE Portfolio DROP CONSTRAINT FK_Portfolio_ToModele;" +
                                            "ALTER TABLE Portfolio_Part DROP CONSTRAINT FK_Portfolio_Part_ToPortfolio;" +
                                            "ALTER TABLE Portfolio_Part DROP CONSTRAINT FK_Portfolio_Part_ToFinancial_Product;" +
                                            "ALTER TABLE Value DROP CONSTRAINT FK_Value_ToFinancial_Product;" +
                                            "ALTER TABLE Value DROP CONSTRAINT FK_Value_ToModele;" +
                                            "ALTER TABLE Value DROP CONSTRAINT FK_Value_ToPortfolio;" +
                                            "TRUNCATE TABLE Value; " +
                                            "TRUNCATE TABLE Share;" +
                                            "TRUNCATE TABLE Financial_Product;" +
                                            "TRUNCATE TABLE Portfolio_Part;" +
                                            "TRUNCATE TABLE Portfolio; " +
                                            "TRUNCATE TABLE Modele; " +
                                            "ALTER TABLE Value ADD CONSTRAINT [FK_Value_ToFinancial_Product] FOREIGN KEY ([financial_product_id]) REFERENCES [dbo].[Financial_Product] ([Id]);" +
                                            "ALTER TABLE Value ADD CONSTRAINT [FK_Value_ToModele] FOREIGN KEY ([error_modele_id]) REFERENCES [dbo].[Modele] ([Id]);" +
                                            "ALTER TABLE Value ADD  CONSTRAINT [FK_Value_ToPortfolio] FOREIGN KEY ([price_portfolio_id]) REFERENCES [dbo].[Portfolio] ([Id]);" +
                                            "ALTER TABLE Portfolio_Part ADD CONSTRAINT [FK_Portfolio_Part_ToPortfolio] FOREIGN KEY ([portfolio_id]) REFERENCES [dbo].[Portfolio] ([Id]);" +
                                            "ALTER TABLE Portfolio_Part ADD CONSTRAINT [FK_Portfolio_Part_ToFinancial_Product] FOREIGN KEY ([financial_product_id]) REFERENCES [dbo].[Financial_Product] ([Id]);" +
                                            "ALTER TABLE Portfolio ADD CONSTRAINT [FK_Portfolio_ToModele] FOREIGN KEY ([modele_id]) REFERENCES [dbo].[Modele] ([Id]);" +
                                            "ALTER TABLE Share ADD CONSTRAINT [FK_Share_ToFinancial_Product] FOREIGN KEY ([Id]) REFERENCES [Financial_Product]([Id]);"
                                            );

            _db.SaveChanges();
            StreamReader monStreamReader = new StreamReader("Z:/Cours/3A/PEPS/Projet/peps_kozei2/peps_kozei2/App_Data/Correspondance.txt");
            string ligne = monStreamReader.ReadLine();
            char[] sep = { ':' };


            while ((ligne = monStreamReader.ReadLine()) != null)
            {
                //nom
                string[] tab;
                tab = ligne.Split(sep);

                Financial_Product f = new Financial_Product();

                Share s = new Share();
                s.Id = f.Id;
                s.country = tab[0];
                s.company = tab[1];
                s.isin_code = tab[2];
                s.ticker = tab[3];
                s.path = tab[4];
                _db.Financial_Product.Add(f);
                _db.Share.Add(s);
                _db.SaveChanges();

            }

            monStreamReader.Close();
            Modele M = new Modele();
            _db.Modele.Add(M);
            Portfolio P = new Portfolio();
            P.modele_id = M.Id;
            _db.Portfolio.Add(P);

            return View("init");
        }

    }
}
